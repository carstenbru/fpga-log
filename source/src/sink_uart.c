/**
 * @file sink_uart.c
 * @brief uart data sink, e.g. as interface to PC
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "sink/sink_uart.h"

void sink_uart_init(sink_uart_t* const sink_uart, formatter_t* const formatter,
		uart_light_regs_t* const uart_light) {
	datastream_object_init(&sink_uart->super); //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	sink_uart->super.update = sink_uart_update;

	sink_uart->control_out = &control_port_dummy;

	sink_uart->data_in = data_port_dummy;
	sink_uart->data_in.parent = (void*) sink_uart;
	sink_uart->data_in.new_data = sink_uart_new_data;

	sink_uart->uart_light = uart_light;

	sink_uart->formatter = formatter;
	formatter_set_write_dest(sink_uart->formatter,
			(void (*)(void *param, unsigned char byte)) uart_light_send,
			sink_uart->uart_light);
}

void sink_uart_set_control_out(sink_uart_t* const sink_uart,
		const control_port_t* const control_in) {
	sink_uart->control_out = control_in;
}

void sink_uart_update(void* const _sink_uart) {
	//nothing to do here for now

	//TODO protocol for pc to invoke control functions of control_out
	//TODO multiple control_out's
}

void sink_uart_new_data(void* const sink_uart,
		const data_package_t* const package) {
	sink_uart_t* sink = (sink_uart_t*) sink_uart;

	sink->formatter->format((void*) (sink->formatter), package);
}
