/**
 * @file formatter_raw.h
 * @brief output log formatter which outputs the received values directly (without additional data, e.g. for GPS messages)
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef FORMATTER_RAW_H_
#define FORMATTER_RAW_H_

#include <fpga-log/sink/formatter/formatter.h>

/** 
 * @brief struct defining a simple formatter which only outputs a message count and the value
 */
typedef struct {
	formatter_t super; 	/**< super-"class": formatter_t */
} formatter_raw_t;

/**
 * @brief init function of formatter_raw
 * 
 * @param formatter	pointer to the formatter struct
 */
void formatter_raw_init(formatter_raw_t* const formatter);

#endif 
