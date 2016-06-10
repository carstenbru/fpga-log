/**
 * @file sink_uart.c
 * @brief uart data sink, e.g. as interface to PC
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/sink/sink_uart.h>

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
 * @param	_sink_uart	pointer to the uart sink
 * @param	package			the incoming data package
 */
static void sink_uart_new_data(void* const _sink_uart,
		const data_package_t* const package);

/**
 * @brief sends a control message to one control out
 *
 * @param _sink_uart	pointer to the sink_uart struct
 * @param output			number of the output to send the message to
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void sink_uart_send_message(void* const _sink_uart, unsigned int output,
		unsigned int count, const control_parameter_t* parameters);

void sink_uart_init(sink_uart_t* const sink_uart, formatter_t* const formatter,
		control_protocol_t* protocol, uart_light_regs_t* const uart_light) {
	datastream_object_init(&sink_uart->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	sink_uart->super.update = sink_uart_update;

	int i;
	for (i = 0; i < SINK_UART_CONTROL_OUT_MAX; i++) {
		sink_uart->control_out[i] = &control_port_dummy;
	}

	sink_uart->data_in = data_port_dummy;
	sink_uart->data_in.parent = (void*) sink_uart;
	sink_uart->data_in.new_data = sink_uart_new_data;

	sink_uart->uart_light = uart_light;

	sink_uart->formatter = formatter;
	_formatter_set_write_dest(sink_uart->formatter,
			(void (*)(void *param, unsigned char byte)) uart_light_send,
			sink_uart->uart_light);

	sink_uart->protocol = protocol;
	control_protocol_set_message_dest(sink_uart->protocol, sink_uart_send_message,
			sink_uart);
}

data_port_t* sink_uart_get_data_in(sink_uart_t* const sink_uart) {
	return &sink_uart->data_in;
}

int sink_uart_add_control_out(sink_uart_t* const sink_uart,
		const control_port_t* const control_in) {
	if (sink_uart->control_out_count < SINK_UART_CONTROL_OUT_MAX) {
		sink_uart->control_out[sink_uart->control_out_count++] = control_in;
		return 1;
	} else
		return 0;
}

static void sink_uart_update(void* const _sink_uart) {
	sink_uart_t* sink_uart = (sink_uart_t*) _sink_uart;

	unsigned char byte;
	if (uart_light_receive_nb(sink_uart->uart_light, &byte) == UART_OK) {
		sink_uart->protocol->parse(sink_uart->protocol, byte);
	}
}

static void sink_uart_new_data(void* const _sink_uart,
		const data_package_t* const package) {
	sink_uart_t* sink = (sink_uart_t*) _sink_uart;

	sink->formatter->format((void*) (sink->formatter), package);
}

static void sink_uart_send_message(void* const _sink_uart, unsigned int output,
		unsigned int count, const control_parameter_t* parameters) {
	sink_uart_t* sink_uart = (sink_uart_t*) _sink_uart;

	if (output <= sink_uart->control_out_count) {
		if (output) {
			const control_port_t* out = sink_uart->control_out[output - 1];
			out->new_control_message(out->parent, count, parameters);
		} else {
			int i;
			for (i = 0; i < sink_uart->control_out_count; i++) {
				const control_port_t* out = sink_uart->control_out[i];
				out->new_control_message(out->parent, count, parameters);
			}
		}
	}
}
