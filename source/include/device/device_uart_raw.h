/**
 * @file device_uart_raw.h
 * @brief simple raw uart logging device driver
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef UART_RAW_H_
#define UART_RAW_H_

#include <uart.h>

#include "datastream_object.h"
#include "data_port.h"
#include "control_port.h"

/** 
 * @brief struct describing a uart raw device 
 */
typedef struct {
	datastream_object_t super;			/**< super-"class": datastream_object_t*/

	const data_port_t* data_out; 		/**< data output destination */
	control_port_t control_in; 			/**< control port, this can be set at a control output to direct the control stream to this device */

	int id; 												/**< id of the device */
	uart_light_regs_t* uart_light; 	/**< pointer to UART hardware registers */
} device_uart_raw_t;

/**
 * @brief uart raw init function
 * 
 * initializes the uart raw device, should be called before using the device
 * 
 * @param	uart_raw	pointer to the uart raw device
 * @param uart_light 	pointer to a uart_light peripheral
 * @param id		id of the new device (for log output, etc)
 */
void device_uart_raw_init(device_uart_raw_t* const uart_raw,
		uart_light_regs_t* const uart_light, const int id);

/**
 * @brief connects the data output port of a uart raw to a given destination
 * 
 * @param	uart_raw	pointer to the uart raw device
 * @param	data_in		the new data destination
 */
void device_uart_raw_set_data_out(device_uart_raw_t* const uart_raw,
		const data_port_t* const data_in);

#endif 
