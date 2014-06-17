/**
 * @file formatter_simple.c
 * @brief simple output log formatter which only outputs a message count and the value
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include "sink/formatter/formatter_simple.h"
#include "long_int.h"
#include "simple_float.h"

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

	print_long(package->timestamp->lpt, 1, 12);
	stdio_descr.send_byte(stdio_descr.base_adr, '.');
	print_long(package->timestamp->hpt, FORMATTER_SIMPLE_HPT_LENGTH,
	FORMATTER_SIMPLE_HPT_LENGTH);
	printf("\tcount %d: %s\tvalue ", fs->count++, package->val_name);
	switch (package->type) {
	case (DATA_TYPE_INT): {
		printf("%d", *((unsigned int* ) package->data));
		break;
	}
	case (DATA_TYPE_BYTE): {
		printf("%d", (int ) *((unsigned char* ) package->data));
		break;
	}
	case (DATA_TYPE_LONG): {
		print_long(*((unsigned long int*) package->data), 1, 12);
		break;
	}
	case (DATA_TYPE_SIMPLE_FLOAT): {
		simple_float_b10_t* f = (simple_float_b10_t*) package->data;
		int i = f->coefficient;
		if (i < 0)
			i = -i;
		printf("  %d.%d", f->coefficient / 10000, i % 10000);
		printf("E%d", f->exponent);
		break;
	}
	default:
		printf("unsupported data type in stream!");
	}
	putchar('\n');
}
