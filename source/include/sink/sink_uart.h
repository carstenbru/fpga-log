/**
 * @file sink_uart.h
 * @brief uart data sink, e.g. as interface to PC
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SINK_UART_H_
#define SINK_UART_H_

#include <uart.h>

#include "data_port.h"
#include "control_port.h"
#include "sink/formatter/formatter.h"

/** 
 * @brief struct describing a uart sink
 */
typedef struct {
	data_port_t data_in; 								/**< data port, this can be set at a data output to direct the data stream to this device */
	const control_port_t* control_out; 	/**< control output port */
	formatter_t* formatter;						  /**< output log formatter */

	uart_light_regs_t* uart_light;		  /**< pointer to UART hardware registers */
} sink_uart_t;

/**
 * @brief uart sink init function
 * 
 * initializes the uart sink, should be called before using the sink
 * 
 * @param	sink_uart		pointer to the uart sink
 * @param	formatter		pointer to a output log formatter
 * @param uart_light 	pointer to a uart_light peripheral
 */
void sink_uart_init(sink_uart_t* const sink_uart, formatter_t* const formatter,
		uart_light_regs_t* const uart_light);

/**
 * @brief connects the control output port of a uart sink to a given destination
 * 
 * @param	sink_uart		pointer to the uart sink
 * @param	control_in	the new data destination
 */
void sink_uart_set_control_out(sink_uart_t* const sink_uart,
		const control_port_t* const control_in);

/**
 * @brief uart sink update function
 * 
 * everything that does not need to be done immediately in an interrupt is done here, e.g. sending control messages to the cotrol_out port
 * 
 * @param	sink_uart	pointer to the uart sink
 */
void sink_uart_update(sink_uart_t* const sink_uart);

/**
 * @brief incoming data function of the uart sink
 *
 * @param	sink_uart	pointer to the uart sink
 * @param	package		the incoming data package
 */
void sink_uart_new_data(void* const sink_uart,
		const data_package_t* const package);

#endif 
