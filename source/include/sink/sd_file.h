/**
 * @file sd_file.h
 * @brief sd card file object
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SD_FILE_H_
#define SD_FILE_H_

#include <fpga-log/sink/formatter/formatter.h>
#include <fpga-log/FatFs/ff.h>

typedef struct {
	formatter_t* formatter; /**< output log (file) formatter */

	FIL file; /**< FatFs file */

	char* filename; /**< the name of the file */
	unsigned int packages_written; /**< amount of packages written since last f_sync */
} sd_file_t;

void sd_file_init(sd_file_t* const file, formatter_t* const formatter,
		const char* filename);

#endif
