/**
 * @file sink_sd_card.c
 * @brief sd card sink, supports FAT filesystems
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/sink/sink_sd_card.h>
#include <stdio.h>

static sink_sd_card_t* pdrv_resolve[_VOLUMES];
static int used_volumes = 0;
const char* sink_sd_fatFS_error_codes[19] = SDCARD_FATFS_ERROR_CODES;
const char* sink_sd_sd_error_codes[3] = SDCARD_SD_ERROR_CODES;

/**
 * sd card write byte function used by formatter
 *
 * @param param	pointer to the sd card sink struct
 * @param byte	the byte to write
 */
static void sink_sd_write_byte(void *param, unsigned char byte);

/**
 * @brief sd card sink update function
 *
 * everything that does not need to be done immediately in an interrupt is done here, e.g. sending control messages to the cotrol_out port
 *
 * @param	_sink_sd_card		pointer to the sd card sink
 */
static void sink_sd_card_update(void* const _sink_sd_card);

/**
 * @brief incoming data function of the sd card sink
 *
 * @param	sink_sd_card	pointer to the sd card sink
 * @param	package				the incoming data package
 * @param which					the number of the input port
 */
static void sink_sd_card_new_data(void* const sink_sd_card,
		const data_package_t* const package, int in_port);
static void sink_sd_card_new_data_0(void* const sink_sd_card,
		const data_package_t* const package);
static void sink_sd_card_new_data_1(void* const sink_sd_card,
		const data_package_t* const package);
static void sink_sd_card_new_data_2(void* const sink_sd_card,
		const data_package_t* const package);
static void sink_sd_card_new_data_3(void* const sink_sd_card,
		const data_package_t* const package);

/**
 * @brief puts a char into the buffer, needed to use printf as sprintf
 *
 * @param buf		the write desination (pointer to an array)
 * @param byte	the byte to write
 */
static void put_buf(void* const param, const unsigned char byte);

/**
 * @brief sd card device send data function
 *
 * @param	_sink_sd_card	pointer to the sd card device
 * @param id						the source id of the pending data
 * @param timestamp			pointer to the timestamp of the pending data
 */
static void sink_sd_card_send_data(void* const _sink_sd_card,
		const unsigned int id, const timestamp_t* const timestamp);

/**
 * checks if the return code is an error and sets the error code in the sink
 *
 * @param sink
 * @param code
 */
static void sink_sd_card_check_return_code(sink_sd_card_t* const sink,
		FRESULT code);

/**
 * @brief opens a file on the sdcard
 *
 * if the file already exists it appends "_x" to the name (e.g. LOG.txt will become LOG_0.txt, LOG_1.txt, ...)
 *
 * @param sink_sd_card
 * @param file
 */
void sink_sd_card_open_file(sink_sd_card_t* const sink_sd_card, sd_file_t* file);

void sink_sd_card_init(sink_sd_card_t* const sink_sd_card,
		sdcard_regs_t* const sd_card, unsigned int sync_interval_packages,
		sd_file_t* file, int id) {
	datastream_source_init(&sink_sd_card->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	sink_sd_card->super.super.update = sink_sd_card_update;
	sink_sd_card->super.send_data = sink_sd_card_send_data;

	unsigned int i;

	sink_sd_card->error_out = &data_port_dummy;

	for (i = 0; i < SDCARD_DATA_IN_MAX; i++) {
		sink_sd_card->data_in[i] = data_port_dummy;
		sink_sd_card->data_in[i].parent = (void*) sink_sd_card;

		sink_sd_card->files[i] = file;
	}
	sink_sd_card->data_in[0].new_data = sink_sd_card_new_data_0;
	sink_sd_card->data_in[1].new_data = sink_sd_card_new_data_1;
	sink_sd_card->data_in[2].new_data = sink_sd_card_new_data_2;
	sink_sd_card->data_in[3].new_data = sink_sd_card_new_data_3;

	sink_sd_card->sdhc_card = 0;
	sink_sd_card->fatFS_error_code = FR_OK;
	sink_sd_card->sd_error_code = SD_NO_ERROR;
	sink_sd_card->sync_interval = sync_interval_packages;
	sink_sd_card->write_file_dest = 0;

	sink_sd_card->pdrv = used_volumes;
	sink_sd_card->status = STA_NOINIT;
	pdrv_resolve[used_volumes++] = sink_sd_card;

	sink_sd_card->sd_card_regs = sd_card;

	_formatter_set_write_dest(file->formatter, sink_sd_write_byte, sink_sd_card);

	char dev_path[3] = { sink_sd_card->pdrv + '0', ':', 0 };
	sink_sd_card_check_return_code(sink_sd_card,
			f_mount(&sink_sd_card->fatFs, dev_path, 0));

	sink_sd_card_open_file(sink_sd_card, file);
}

void sink_sd_card_open_file(sink_sd_card_t* const sink_sd_card, sd_file_t* file) {
	FRESULT open_code;
	if ((open_code = f_open(&file->file, file->filename,
	FA_WRITE | FA_CREATE_NEW)) == FR_EXIST) {  //try to open file, if it already exists go on with appending a number
		unsigned int i;
		char format_string[SDCARD_MAX_FILE_NAME_LENGTH + 1];  //generate format string
		char buf[SDCARD_MAX_FILE_NAME_LENGTH + 1];  //filename buffer
		for (i = 0; i < SDCARD_MAX_FILE_NAME_LENGTH + 1; i++) {  //clear filename and format strings
			buf[i] = 0;
			format_string[i] = 0;
		}

		const char* c = file->filename;
		for (i = 0; i < SDCARD_MAX_FILE_NAME_LENGTH; i++) {
			if (*c == '.') {  //search for the file extension
				format_string[i++] = '_';
				format_string[i++] = '%';
				format_string[i++] = 'd';
			}
			format_string[i] = *c;  //copy string
			if (*c == 0)  //end of string
				break;
			c++;
		}

		io_descr_t old_stdio = stdio_descr;  //redirect stdio for printf call
		stdio_descr.send_byte = put_buf;

		i = 0;
		do {  //generate file with appended "_x" and try to open it until it works
			char* p = buf;
			stdio_descr.base_addr_out = &p;
			printf(format_string, i);
			i++;
		} while ((open_code = f_open(&file->file, (char*) buf,
		FA_WRITE | FA_CREATE_NEW)) == FR_EXIST);

		stdio_descr = old_stdio;
	}
	sink_sd_card_check_return_code(sink_sd_card, open_code);
}

void sink_sd_card_set_error_out(sink_sd_card_t* const sink_sd_card,
		const data_port_t* const data_in) {
	sink_sd_card->error_out = data_in;
}

data_port_t* sink_sd_card_get_data_in_0(sink_sd_card_t* const sink_sd_card) {
	return &sink_sd_card->data_in[0];
}
data_port_t* sink_sd_card_get_data_in_1(sink_sd_card_t* const sink_sd_card) {
	return &sink_sd_card->data_in[1];
}
data_port_t* sink_sd_card_get_data_in_2(sink_sd_card_t* const sink_sd_card) {
	return &sink_sd_card->data_in[2];
}
data_port_t* sink_sd_card_get_data_in_3(sink_sd_card_t* const sink_sd_card) {
	return &sink_sd_card->data_in[3];
}

static void sink_sd_write_byte(void *param, unsigned char byte) {
	sink_sd_card_t* sink = (sink_sd_card_t*) param;

	UINT bw;
	sink_sd_card_check_return_code(sink,
			f_write(&sink->files[sink->write_file_dest]->file, &byte, 1, &bw));
}

static void sink_sd_card_update(void* const _sink_sd_card) {
	sink_sd_card_t* sink = (sink_sd_card_t*) _sink_sd_card;

	unsigned int i;
	for (i = 0; i < SDCARD_DATA_IN_MAX; i++) {
		sd_file_t* file = sink->files[i];
		if (file->packages_written >= sink->sync_interval) {
			sink_sd_card_check_return_code(sink, f_sync(&file->file));
			file->packages_written = 0;
			break;
		}
	}

	if (((sink->sd_error_code & 0b11111100) != SD_NO_ERROR)
			| (sink->fatFS_error_code != FR_OK)) {
		_datastream_source_generate_software_timestamp((datastream_source_t*) sink);
	}
}

static void sink_sd_card_new_data(void* const sink_sd_card,
		const data_package_t* const package, int in_port) {
	sink_sd_card_t* sink = (sink_sd_card_t*) sink_sd_card;

	sink->write_file_dest = in_port;
	sd_file_t* file = sink->files[in_port];
	file->packages_written++;
	file->formatter->format((void*) (file->formatter), package);
}

static void sink_sd_card_new_data_0(void* const sink_sd_card,
		const data_package_t* const package) {
	sink_sd_card_new_data(sink_sd_card, package, 0);
}

static void sink_sd_card_new_data_1(void* const sink_sd_card,
		const data_package_t* const package) {
	sink_sd_card_new_data(sink_sd_card, package, 1);
}

static void sink_sd_card_new_data_2(void* const sink_sd_card,
		const data_package_t* const package) {
	sink_sd_card_new_data(sink_sd_card, package, 2);
}

static void sink_sd_card_new_data_3(void* const sink_sd_card,
		const data_package_t* const package) {
	sink_sd_card_new_data(sink_sd_card, package, 3);
}

static void put_buf(void* const param, const unsigned char byte) {
	*(*(unsigned char**) param)++ = byte;
}

sink_sd_card_t* sink_sd_card_from_pdrv(BYTE pdrv) {
	if (pdrv < used_volumes)
		return pdrv_resolve[pdrv];
	else
		return 0;
}

static void sink_sd_card_send_data(void* const _sink_sd_card,
		const unsigned int id, const timestamp_t* const timestamp) {
	sink_sd_card_t* sink = (sink_sd_card_t*) _sink_sd_card;
	unsigned char byte = 0;

	if (sink->fatFS_error_code != FR_OK) {
		data_package_t package = { id,
				sink_sd_fatFS_error_codes[sink->fatFS_error_code - 1], DATA_TYPE_BYTE,
				&byte, timestamp };
		sink->error_out->new_data(sink->error_out->parent, &package);
	}

	if (sink->sd_error_code != SD_NO_ERROR) {	//TODO
		int i;
		for (i = 0; i < 3; i++) {  //check for init,read and write errors (see hardware specification)
			byte = (sink->sd_error_code >> (i * 2)) & 3;
			if (byte) {
				data_package_t package = { id, sink_sd_sd_error_codes[i],
						DATA_TYPE_BYTE, &byte, timestamp };
				sink->error_out->new_data(sink->error_out->parent, &package);
			}
		}
	}

	sink->sd_error_code &= 3;  //clear errors except init errors
	sink->fatFS_error_code = FR_OK;
}

static void sink_sd_card_check_return_code(sink_sd_card_t* const sink,
		FRESULT code) {
	if (code != FR_OK) {
		if (sink->fatFS_error_code == FR_OK) {  //do not overwrite previous errors
			sink->fatFS_error_code = code;
		}
	}
}

void sink_sd_card_set_file_for_input_port(sink_sd_card_t* const sink_sd_card,
		sd_file_t* file, unsigned int port_number) {
	if (port_number < SDCARD_DATA_IN_MAX) {
		sink_sd_card->files[port_number] = file;
		_formatter_set_write_dest(file->formatter, sink_sd_write_byte,
				sink_sd_card);
		sink_sd_card_open_file(sink_sd_card, file);
	}
}
