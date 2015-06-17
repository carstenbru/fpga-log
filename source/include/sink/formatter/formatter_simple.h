/**
 * @file formatter_simple.h
 * @brief simple output log formatter which only outputs a message count and the value
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef FORMATTER_SIMPLE_H_
#define FORMATTER_SIMPLE_H_

#include <fpga-log/sink/formatter/formatter.h>

/**
 * @brief high prescision timestamp digits in log
 */
#define FORMATTER_SIMPLE_HPT_LENGTH 9

typedef enum {
	FORMATTER_SIMPLE_NO_SOURCE_ID, FORMATTER_SIMPLE_PRINT_SOURCE_ID,
} formatter_simple_id_option;

typedef enum {
	FORMATTER_SIMPLE_USE_TABS, /**< use tabulator in output to separate different values (spaces will be used between value name and value) */
	FORMATTER_SIMPLE_NO_TABS, /**< use only spaces in output to separate between different values and value name from values */
	FORMATTER_SIMPLE_ONLY_TABS, /**< use only tabulators in output to separate different values and names from values */
} formatter_simple_tab_mode;

/** 
 * @brief struct defining a simple formatter which only outputs a message count and the value
 */
typedef struct {
	formatter_t super; /**< super-"class": formatter_t */

	int count; /**< message counter */

	formatter_simple_id_option print_source_id;
	formatter_simple_tab_mode tab_mode;
} formatter_simple_t;

/**
 * @brief init function of formatter_simple
 * 
 * @param formatter	pointer to the formatter struct
 */
void formatter_simple_init(formatter_simple_t* const formatter,
		formatter_simple_id_option print_source_id,
		formatter_simple_tab_mode tab_mode);

#endif 
