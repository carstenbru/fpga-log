/**
 * @file formatter.c
 * @brief output log formatter
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include <fpga-log/sink/formatter/formatter_simple.h>

void formatter_set_write_dest(formatter_t* const formatter,
		void (*write_byte)(void* const param, const unsigned char byte),
		void* const param) {
	formatter->write_byte = write_byte;
	formatter->param = param;
}
