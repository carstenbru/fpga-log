 /**
 * @file formatter.h
 * @brief output log formatter
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */


#ifndef FORMATTER_H_
#define FORMATTER_H_

#include "data_port.h"

/** 
 * @brief struct defining a formatter with its state and functions
 */
typedef struct {
  /**
   * @brief pointer to the format function of one implementation
   * 
   * @param	formatter	the formatter struct
   * @param	package		the incoming data package
   */
  void (*format)(void* formatter, data_package_t* package);
  
  /**
   * @brief pointer to the function which will be used to write bytes
   * 
   * @param	param		a (fixed) paramter which can be set with pointer "param", e.g. the peripheral
   * @param	byte		the byte to write
   */
  void (*write_byte)(void *param, unsigned char byte);

  void* param;			/**< parameter which will be passed to write_byte function */
} formatter_t;

/**
 * @brief stets the write destination of the formatter
 * 
 * @param	write_byte	function which will be called to write bytes
 * @param	param		parameter which will be passed to write_byte function
 */
void formatter_set_write_dest(
    formatter_t* formatter,
    void (*write_byte)(void *param, unsigned char byte),
    void* param);

#endif 
