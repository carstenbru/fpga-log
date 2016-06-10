/**
 * @file sd_file.c
 * @brief sd card file object
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/sink/sd_file.h>

void sd_file_init(sd_file_t* const file, formatter_t* const formatter,
		const char* filename) {
	file->formatter = formatter;
	file->filename = filename;
	file->packages_written = 0;
}
