/**
 * @file formatter_simple.h
 * @brief simple output log formatter which only outputs a message count and the value
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef FORMATTER_SIMPLE_H_
#define FORMATTER_SIMPLE_H_

#include "sink/formatter/formatter.h"

/** 
 * @brief struct defining a simple formatter which only outputs a message count and the value
 */
typedef struct {
	formatter_t super; 	/**< super-"class": formatter_t */

	int count; 					/**< message counter */
} formatter_simple_t;

/**
 * @brief init function of formatter_simple
 * 
 * @param formatter	pointer to the formatter struct
 */
void formatter_simple_init(formatter_simple_t* const formatter);

/**
 * @brief simple formatter output log format function
 * 
 * @param	formatter	pointer to the formatter struct
 * @param	package		the incoming package
 */
void formatter_simple_format(void* const formatter,
		const data_package_t* const package);

#endif 
