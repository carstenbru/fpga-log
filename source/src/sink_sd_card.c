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
 */
static void sink_sd_card_new_data(void* const sink_sd_card,
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

void sink_sd_card_init(sink_sd_card_t* const sink_sd_card,
		formatter_t* const formatter, sdcard_regs_t* const sd_card,
		unsigned int sync_interval_packages, int id) {
	datastream_source_init(&sink_sd_card->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	sink_sd_card->super.super.update = sink_sd_card_update;
	sink_sd_card->super.send_data = sink_sd_card_send_data;

	sink_sd_card->error_out = &data_port_dummy;
	sink_sd_card->data_in = data_port_dummy;
	sink_sd_card->data_in.parent = (void*) sink_sd_card;
	sink_sd_card->data_in.new_data = sink_sd_card_new_data;

	sink_sd_card->fatFS_error_code = FR_OK;
	sink_sd_card->sd_error_code = SD_NO_ERROR;
	sink_sd_card->sync_interval = sync_interval_packages;

	sink_sd_card->pdrv = used_volumes;
	sink_sd_card->status = STA_NOINIT;
	pdrv_resolve[used_volumes++] = sink_sd_card;

	sink_sd_card->sd_card_regs = sd_card;

	sink_sd_card->formatter = formatter;
	_formatter_set_write_dest(formatter, sink_sd_write_byte, sink_sd_card);

	char dev_path[3] = { sink_sd_card->pdrv + '0', ':', 0 };
	sink_sd_card_check_return_code(sink_sd_card,
			f_mount(&sink_sd_card->fatFs, dev_path, 0));

	io_descr_t old_stdio = stdio_descr;
	unsigned char buf[SDCARD_MAX_FILE_NAME_LENGTH + 1];
	unsigned int i;
	for (i = 0; i < SDCARD_MAX_FILE_NAME_LENGTH + 1; i++)
		buf[i] = 0;
	i = 0;
	stdio_descr.send_byte = put_buf;
	FRESULT open_code;
	do {
		unsigned char* p = buf;
		stdio_descr.base_adr = &p;
		printf(SDCARED_FILE_NAME, i);
		i++;
	} while ((open_code = f_open(&sink_sd_card->file, (char*) buf,
	FA_WRITE | FA_CREATE_NEW)) == FR_EXIST);
	sink_sd_card_check_return_code(sink_sd_card, open_code);
	stdio_descr = old_stdio;
}

void sink_sd_card_set_error_out(sink_sd_card_t* const sink_sd_card,
		const data_port_t* const data_in) {
	sink_sd_card->error_out = data_in;
}

data_port_t* sink_sd_card_get_data_in(sink_sd_card_t* const sink_sd_card) {
	return &sink_sd_card->data_in;
}

static void sink_sd_write_byte(void *param, unsigned char byte) {
	sink_sd_card_t* sink = (sink_sd_card_t*) param;

	UINT bw;
	sink_sd_card_check_return_code(sink, f_write(&sink->file, &byte, 1, &bw));
}

static void sink_sd_card_update(void* const _sink_sd_card) {
	sink_sd_card_t* sink = (sink_sd_card_t*) _sink_sd_card;

	//set error code here again (it is also set in sdcard peripheral methods) so previous hardware errors are
	//still reported if fatFS does no hardware access anymore due to the previous error
	sink->sd_error_code |= sink->sd_card_regs->trans_error;

	if (sink->packages_written >= sink->sync_interval) {
		sink_sd_card_check_return_code(sink, f_sync(&sink->file));
		sink->packages_written = 0;
	}

	if ((sink->sd_error_code != SD_NO_ERROR)
			| (sink->fatFS_error_code != FR_OK)) {
		_datastream_source_generate_software_timestamp((datastream_source_t*) sink);
	}
}

static void sink_sd_card_new_data(void* const sink_sd_card,
		const data_package_t* const package) {
	sink_sd_card_t* sink = (sink_sd_card_t*) sink_sd_card;

	sink->packages_written++;
	sink->formatter->format((void*) (sink->formatter), package);
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

	if (sink->sd_error_code != SD_NO_ERROR) {
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

	sink->sd_error_code = SD_NO_ERROR;
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
