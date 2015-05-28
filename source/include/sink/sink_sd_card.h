/**
 * @file sink_sd_card.h
 * @brief sd card sink, supports FAT filesystems
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SINK_SD_CARD_H_
#define SINK_SD_CARD_H_

#include <peripherals/sdcard.h>

#include <fpga-log/FatFs/ff.h>
#include <fpga-log/FatFs/diskio.h>

#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>
#include <fpga-log/sink/formatter/formatter.h>
#include <fpga-log/sink/sd_file.h>

/**
 * @brief defines the maximal usable file name length of the log file
 */
#define SDCARD_MAX_FILE_NAME_LENGTH 16

#define SDCARD_FATFS_ERROR_CODES {"SD card error", "int error", "sd card not ready", "file not found", "path not found","invalid path","access denied",\
	"file already exists","invalid object","card write protected","invalid drive","not enabled","no filesystem found", "mkfs error","SD timeout",\
	"locked file","not enough core","too many open files","invalid parameter"}

#define SDCARD_SD_ERROR_CODES {"SD init error", "SD read error", "SD write error"}

#define SDCARD_DATA_IN_MAX 4

/**
 * @brief struct describing a sd card sink
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	data_port_t data_in[SDCARD_DATA_IN_MAX]; /**< data port, this can be set at a data output to direct the data stream to this device */
	const data_port_t* error_out; /**< error output destination */

	BYTE pdrv; /**< pdrv ID used by FatFs */
	DSTATUS status; /**< sd card status */
	FATFS fatFs; /**< FatFs volume */

	sd_file_t* files[SDCARD_DATA_IN_MAX]; /**< pointers to file objects */

	FRESULT fatFS_error_code; /**< last FatFs error code */
	unsigned char sd_error_code; /**< last SD card error code */
	unsigned int sync_interval; /**< amount of packages until f_sync should be called */

	unsigned int write_file_dest; /**< indicator for write_byte function which file should be used */

	sdcard_regs_t* sd_card_regs; /**< pointer to SD-card hardware registers */
	unsigned char sdhc_card;
} sink_sd_card_t;

/**
 * @brief sd card sink init function
 *
 * initializes the sd card sink, should be called before using the sink
 *
 * @param	sink_sd_card						pointer to the sd card sink
 * @param	formatter								pointer to a output log formatter
 * @param	sd_card							 		pounter to the SD-card peripheral
 * @param sync_interval_packages	amount of packages until f_sync should be called
 * @param file										the file where the data should be stored
 */
void sink_sd_card_init(sink_sd_card_t* const sink_sd_card,
		sdcard_regs_t* const sd_card, unsigned int sync_interval_packages,
		sd_file_t* file, int id);

/**
 * @brief connects the error output port of a sd card to a given destination
 *
 * @param	sink_sd_card	pointer to the sd card sink
 * @param	data_in		the new data destination
 */
void sink_sd_card_set_error_out(sink_sd_card_t* const sink_sd_card,
		const data_port_t* const data_in);

/**
 * @brief returns the sd card sink data input
 *
 * @param sink_sd_card	pointer to the sd card sink
 * @return the data input port
 */
data_port_t* sink_sd_card_get_data_in_0(sink_sd_card_t* const sink_sd_card);
data_port_t* sink_sd_card_get_data_in_1(sink_sd_card_t* const sink_sd_card);
data_port_t* sink_sd_card_get_data_in_2(sink_sd_card_t* const sink_sd_card);
data_port_t* sink_sd_card_get_data_in_3(sink_sd_card_t* const sink_sd_card);

/**
 * @brief associates a file with a module input, by default all inputs log to the default file
 *
 * @param file	new file
 * @param port_number	port the file should be assigned to
 */
void sink_sd_card_set_file_for_input_port(sink_sd_card_t* const sink_sd_card,
		sd_file_t* file, unsigned int port_number);

/**
 * gets the sd card sink struct from FatFs pdrev id
 *
 * @param pdrv	id used by FatFs
 * @return	pointer to the sd card sink struct
 */
sink_sd_card_t* sink_sd_card_from_pdrv(BYTE pdrv);

#endif
