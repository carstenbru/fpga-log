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
 * @brief struct defining a formatter with its state and fucntions
 */
typedef struct {
  void* formatter_state;	/**< pointer to the formatter state struct (one implementation) */
  void* param;			/**< parameter which will be passed to write_byte function
  
  /**
   * @brief pointer to the format function of one implementation
   * 
   * @param	state		the state stored in this struct
   * @param	package		the incoming data package
   */
  void (*format)(void* formatter, data_package_t* package);
  
  /**
   * @brief pointer to the function which should be used to write bytes
   * 
   * @param	param		a (fixed) paramter which can be set with pointer "param", e.g. the peripheral
   * @param	byte		the byte to write
   */
  void (*write_byte)(void *param, unsigned char byte);
} formatter_t;

void formatter_test(void* formatter, data_package_t* package);

#endif 
