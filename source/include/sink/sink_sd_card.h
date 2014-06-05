/**
 * @file sink_sd_card.h
 * @brief sd card sink, supports FAT filesystems
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SINK_SD_CARD_H_
#define SINK_SD_CARD_H_

#include <peripherals/sdcard.h>

#include "FatFs/ff.h"
#include "FatFs/diskio.h"

#include "datastream_object.h"
#include "data_port.h"
#include "sink/formatter/formatter.h"

/**
 * @brief defines the maximal usable file name length of the log file
 */
#define SDCARD_MAX_FILE_NAME_LENGTH 16

/**
 * @brief defines the name of the log file, %d can be used as number of log file
 */
#define SDCARED_FILE_NAME "log_%d.txt"

/**
 * @brief struct describing a sd card sink
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/

	data_port_t data_in; /**< data port, this can be set at a data output to direct the data stream to this device */

	formatter_t* formatter; /**< output log formatter */

	BYTE pdrv; /**< pdrv ID used by FatFs */
	DSTATUS status; /**< sd card status */
	FATFS fatFs; /**< FatFs volume */
	FIL file; /**< FatFs file */

	sdcard_regs_t* sd_card_regs; /**< pointer to SD-card hardware registers */
} sink_sd_card_t;

/**
 * @brief sd card sink init function
 *
 * initializes the sd card sink, should be called before using the sink
 *
 * @param	sink_sd_card	pointer to the sd card sink
 * @param	formatter			pointer to a output log formatter
 * @param	sd_card				pounter to the SD-card peripheral
 */
void sink_sd_card_init(sink_sd_card_t* const sink_sd_card,
		formatter_t* const formatter, sdcard_regs_t* const sd_card);

/**
 * gets the sd card sink struct from FatFs pdrev id
 *
 * @param pdrv	id used by FatFs
 * @return	pointer to the sd card sink struct
 */
sink_sd_card_t* sink_sd_card_from_pdrv(BYTE pdrv);

#endif
