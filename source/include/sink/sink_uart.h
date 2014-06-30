/**
 * @file sink_uart.h
 * @brief uart data sink, e.g. as interface to PC
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SINK_UART_H_
#define SINK_UART_H_

#include <uart.h>

#include "datastream_object.h"
#include "data_port.h"
#include "control_port.h"
#include "sink/formatter/formatter.h"
#include "sink/control_protocol.h"

/**
 * @brief number of maximal control outs for a uart sink
 *
 * Since we have no heap (malloc) on SpartanMC we need to allocate a fixed amount of memory with the struct.
 */
#define SINK_UART_CONTROL_OUT_MAX 8

/** 
 * @brief struct describing a uart sink
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/

	data_port_t data_in; /**< data port, this can be set at a data output to direct the data stream to this device */
	const control_port_t* control_out[SINK_UART_CONTROL_OUT_MAX]; /**< control output ports */
	int control_out_count; /**< currently assigned control outs */

	formatter_t* formatter; /**< output log formatter */
	control_protocol_t* protocol; /**< control protocol */

	uart_light_regs_t* uart_light; /**< pointer to UART hardware registers */
} sink_uart_t;

/**
 * @brief uart sink init function
 * 
 * initializes the uart sink, should be called before using the sink
 * 
 * @param	sink_uart		pointer to the uart sink
 * @param	formatter		pointer to a output log formatter
 * @param protocol		pointer to a control protocol
 * @param uart_light 	pointer to a uart_light peripheral
 */
void sink_uart_init(sink_uart_t* const sink_uart, formatter_t* const formatter,
		control_protocol_t* protocol, uart_light_regs_t* const uart_light);

/**
 * @brief returns the uart sink data input
 *
 * @param sink_uart		pointer to the uart sink
 * @return the data input port
 */
data_port_t* sink_uart_get_data_in(sink_uart_t* const sink_uart);

/**
 * @brief connects the next control output port of a uart sink to a given destination
 * 
 * @param	sink_uart		pointer to the uart sink
 * @param	control_in	the new control destination
 *
 * @return 1 on success, otherwise 0 (e.g. max destinations already reached)
 */
int sink_uart_add_control_out(sink_uart_t* const sink_uart,
		const control_port_t* const control_in);

#endif 
