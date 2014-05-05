/**
 * @file formatter_simple.c
 * @brief simple output log formatter which only outputs a message count and the value
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include "sink/formatter/formatter_simple.h"

void formatter_simple_init(formatter_simple_t* const formatter) {
	formatter_t* f = (formatter_t*) formatter;

	/**
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	f->format = formatter_simple_format;

	formatter->count = 0;
}

void formatter_simple_format(void* const formatter,
		const data_package_t* const package) {
	formatter_t* f = (formatter_t*) formatter;
	stdio_descr.base_adr = f->param;
	stdio_descr.send_byte = f->write_byte;

	formatter_simple_t* fs = (formatter_simple_t*) formatter;

	printf("%d: %d\n\r", fs->count++, *((unsigned char*) package->data));
}
