/**
 * @file formatter.h
 * @brief abstract super-"class" for output log formatters
 * 
 * This should be used as super-"class" for formatter implementations.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef FORMATTER_H_
#define FORMATTER_H_

#include <fpga-log/data_port.h>

/** 
 * @brief struct defining a formatter with its state and functions
 *
 * This should be used as super-"class" for formatter implementations.
 */
typedef struct {
	/**
	 * @brief pointer to the format function of one implementation
	 *
	 * @param	formatter	the formatter struct
	 * @param	package		the incoming data package
	 */
	void (*format)(void* const formatter, const data_package_t* const package);

	/**
	 * @brief pointer to the function which will be used to write bytes
	 *
	 * @param	param		a (fixed) paramter which can be set with pointer "param", e.g. the peripheral
	 * @param	byte		the byte to write
	 */
	void (*write_byte)(void* const param, const unsigned char byte);

	void* param; /**< parameter which will be passed to write_byte function */
} formatter_t;

/**
 * @brief stets the write destination of the formatter
 * 
 * @param formatter		pointer to the formatter
 * @param	write_byte	function which will be called to write bytes
 * @param	param				parameter which will be passed to write_byte function
 */
void _formatter_set_write_dest(formatter_t* const formatter,
		void (*write_byte)(void* const param, const unsigned char byte),
		void* const param);

#endif 
