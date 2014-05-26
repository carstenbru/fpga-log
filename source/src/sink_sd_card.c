/**
 * @file sink_sd_card.c
 * @brief sd card sink, supports FAT filesystems
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "sink/sink_sd_card.h"

static sink_sd_card_t* pdrev_resolve[_VOLUMES];
static int used_volumes = 0;

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

void sink_sd_card_init(sink_sd_card_t* const sink_sd_card,
		formatter_t* const formatter, sdcard_regs_t* const sd_card) {
	datastream_object_init(&sink_sd_card->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	sink_sd_card->super.update = sink_sd_card_update;

	sink_sd_card->data_in = data_port_dummy;
	sink_sd_card->data_in.parent = (void*) sink_sd_card;
	sink_sd_card->data_in.new_data = sink_sd_card_new_data;

	sink_sd_card->pdrv = used_volumes;
	sink_sd_card->status = STA_NOINIT;
	pdrev_resolve[used_volumes++] = sink_sd_card;

	sink_sd_card->sd_card_regs = sd_card;

	sink_sd_card->formatter = formatter;
	formatter_set_write_dest(formatter, sink_sd_write_byte, sink_sd_card);

	char dev_path[3] = { sink_sd_card->pdrv + '0', ':', 0 };
	f_mount(&sink_sd_card->fatFs, dev_path, 0);
	//TODO variable file name, perhaps multiple files?
	f_open(&sink_sd_card->file, "fpga-log.txt", FA_WRITE | FA_CREATE_ALWAYS);

	//TODO when close file/sync so that no data gets lost?
}

static void sink_sd_write_byte(void *param, unsigned char byte) {
	sink_sd_card_t* sink = (sink_sd_card_t*) param;

	UINT bw;
	f_write(&sink->file, &byte, 1, &bw);
}

static void sink_sd_card_update(void* const _sink_sd_card) {
	sink_sd_card_t* sink = (sink_sd_card_t*) _sink_sd_card;

	f_sync(&sink->file); //TODO flush not on every update, perhaps use 2000hz timer?
}

static void sink_sd_card_new_data(void* const sink_sd_card,
		const data_package_t* const package) {
	sink_sd_card_t* sink = (sink_sd_card_t*) sink_sd_card;

	sink->formatter->format((void*) (sink->formatter), package);
}

sink_sd_card_t* sink_sd_card_from_pdrv(BYTE pdrv) {
	if (pdrv < used_volumes)
		return pdrev_resolve[pdrv];
	else
		return 0;
}
