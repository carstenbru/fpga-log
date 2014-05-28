/**
 * @file sink_uart.c
 * @brief uart data sink, e.g. as interface to PC
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "sink/sink_uart.h"

/**
 * @brief uart sink update function
 *
 * everything that does not need to be done immediately in an interrupt is done here, e.g. sending control messages to the cotrol_out port
 *
 * @param	_sink_uart	pointer to the uart sink
 */
static void sink_uart_update(void* const _sink_uart);

/**
 * @brief incoming data function of the uart sink
 *
 * @param	sink_uart	pointer to the uart sink
 * @param	package		the incoming data package
 */
static void sink_uart_new_data(void* const sink_uart,
		const data_package_t* const package);

void sink_uart_init(sink_uart_t* const sink_uart, formatter_t* const formatter,
		uart_light_regs_t* const uart_light) {
	datastream_object_init(&sink_uart->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	sink_uart->super.update = sink_uart_update;

	int i;
	for (i = 0; i < SINK_UART_MAX_CONTROL_OUTS; i++) {
		sink_uart->control_out[i] = &control_port_dummy;
	}

	sink_uart->data_in = data_port_dummy;
	sink_uart->data_in.parent = (void*) sink_uart;
	sink_uart->data_in.new_data = sink_uart_new_data;

	sink_uart->uart_light = uart_light;

	sink_uart->formatter = formatter;
	formatter_set_write_dest(sink_uart->formatter,
			(void (*)(void *param, unsigned char byte)) uart_light_send,
			sink_uart->uart_light);
}

int sink_uart_add_control_out(sink_uart_t* const sink_uart,
		const control_port_t* const control_in) {
	if (sink_uart->control_out_count < SINK_UART_MAX_CONTROL_OUTS) {
		sink_uart->control_out[sink_uart->control_out_count++] = control_in;
		return 1;
	} else
		return 0;
}

static void sink_uart_update(void* const _sink_uart) {
	//nothing to do here for now

	//TODO protocol for pc to invoke control functions of control_out
}

static void sink_uart_new_data(void* const sink_uart,
		const data_package_t* const package) {
	sink_uart_t* sink = (sink_uart_t*) sink_uart;

	sink->formatter->format((void*) (sink->formatter), package);
}
