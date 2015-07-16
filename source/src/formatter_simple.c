/**
 * @file formatter_simple.c
 * @brief simple output log formatter which only outputs a message count and the value
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include <fpga-log/sys_init.h>
#include <fpga-log/sink/formatter/formatter_simple.h>
#include <fpga-log/long_int.h>
#include <fpga-log/simple_float.h>

/**
 * @brief simple formatter output log format function
 *
 * @param	formatter	pointer to the formatter struct
 * @param	package		the incoming package
 */
static void formatter_simple_format(void* const formatter,
		const data_package_t* const package);

void formatter_simple_init(formatter_simple_t* const formatter,
		formatter_simple_id_option print_source_id,
		formatter_simple_tab_mode tab_mode,
		formatter_simple_timestamp_mode timestamp_mode) {
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	formatter->super.format = formatter_simple_format;

	formatter->count = 0;
	formatter->print_source_id = print_source_id;
	formatter->tab_mode = tab_mode;
	formatter->timestamp_mode = timestamp_mode;

	if (timestamp_mode == FORMATTER_SIMPLE_TIMESTAMP_US) {
		formatter->ts_divider = get_peri_clock() / 1000000;
	}
}

static void formatter_simple_format(void* const formatter,
		const data_package_t* const package) {
	formatter_t* f = (formatter_t*) formatter;
	stdio_descr.base_addr_out = f->param;
	stdio_descr.send_byte = f->write_byte;

	formatter_simple_t* fs = (formatter_simple_t*) formatter;

	if (fs->print_source_id == FORMATTER_SIMPLE_PRINT_SOURCE_ID) {
		if (fs->tab_mode == FORMATTER_SIMPLE_USE_TABS) {
			printf("ID%d\t", package->source_id);
		} else if (fs->tab_mode == FORMATTER_SIMPLE_NO_TABS) {
			printf("ID%d ", package->source_id);
		} else {
			printf("ID%d\t", package->source_id);
		}
	}

	print_long(package->timestamp->lpt_union.lpt, 1, 12);
	stdio_descr.send_byte(stdio_descr.base_addr_out, '.');

	if (fs->timestamp_mode == FORMATTER_SIMPLE_TIMESTAMP_RAW) {
		print_long(package->timestamp->hpt_union.hpt, FORMATTER_SIMPLE_HPT_LENGTH,
		FORMATTER_SIMPLE_HPT_LENGTH);
	} else {
		unsigned long hpt = package->timestamp->hpt_union.hpt;
		hpt /= fs->ts_divider;
		print_long(hpt, 6, 6);
	}

	if (fs->tab_mode == FORMATTER_SIMPLE_USE_TABS) {
		printf("\tcount %d: %s\tvalue ", fs->count++, package->val_name);
	} else if (fs->tab_mode == FORMATTER_SIMPLE_NO_TABS) {
		printf(" count %d: %s value ", fs->count++, package->val_name);
	} else {
		printf("\tcount\t%d:\t%s\tvalue\t", fs->count++, package->val_name);
	}
	switch (package->type) {
	case (DATA_TYPE_INT): {
		printf("%d", *((int* ) package->data));
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
		long i = f->coefficient;
		if (i < 0)
			i = -i;
		print_long(f->coefficient / cast_to_ulong(10000), 1, 12);
		printf(".%04d", i % cast_to_ulong(10000));
		if (f->exponent)
			printf("E%d", f->exponent);
		break;
	}
	case (DATA_TYPE_CHAR): {
		putchar(*((unsigned char*) package->data));
		break;
	}
	default:
		printf("unsupported data type in stream!\n");
	}
	putchar('\n');
}
