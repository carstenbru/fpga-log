/**
 * @file formatter_simple.c
 * @brief simple output log formatter which only outputs a message count and the value
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include "sink/formatter/formatter_simple.h"

/**
 * @brief simple formatter output log format function
 *
 * @param	formatter	pointer to the formatter struct
 * @param	package		the incoming package
 */
static void formatter_simple_format(void* const formatter,
		const data_package_t* const package);

void formatter_simple_init(formatter_simple_t* const formatter) {
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	formatter->super.format = formatter_simple_format;

	formatter->count = 0;
}

static void formatter_simple_format(void* const formatter,
		const data_package_t* const package) {
	formatter_t* f = (formatter_t*) formatter;
	stdio_descr.base_adr = f->param;
	stdio_descr.send_byte = f->write_byte;

	formatter_simple_t* fs = (formatter_simple_t*) formatter;

	printf("%d: %d\n", fs->count++, (int)*((unsigned char*) package->data));
}
