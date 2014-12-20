/**
 * @file formatter_raw.c
 * @brief output log formatter which outputs the received values directly (without additional data, e.g. for GPS messages)
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include <fpga-log/sink/formatter/formatter_raw.h>
#include <fpga-log/long_int.h>
#include <fpga-log/simple_float.h>

/**
 * @brief raw formatter output log format function
 *
 * @param	formatter	pointer to the formatter struct
 * @param	package		the incoming package
 */
static void formatter_raw_format(void* const formatter,
		const data_package_t* const package);

void formatter_raw_init(formatter_raw_t* const formatter) {
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	formatter->super.format = formatter_raw_format;
}

static void formatter_raw_format(void* const formatter,
		const data_package_t* const package) {
	formatter_t* f = (formatter_t*) formatter;
	stdio_descr.base_adr = f->param;
	stdio_descr.send_byte = f->write_byte;

	switch (package->type) {
	case (DATA_TYPE_BYTE): {
		f->write_byte(f->param, *((unsigned char* ) package->data));
		break;
	}
	default:
		printf("unsupported data type in stream!\n");
	}
}
