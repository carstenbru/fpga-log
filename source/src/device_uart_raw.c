/**
 * @file device_uart_raw.c
 * @brief simple raw uart logging device driver
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "device/device_uart_raw.h"
#include "peripheral_funcs/uart_light_funcs.h"

/**
 * @brief uart raw send data function
 *
 * This function sends the data packages to the devices output port.
 *
 * @param	_uart_raw		pointer to the uart raw device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_uart_raw_send_data(void* const _uart_raw,
		const unsigned int id, const timestamp_t* const timestamp);

/**
 * @brief control message function of uart raw
 *
 * @param	uart_raw		the uart_raw device
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void device_uart_raw_new_control_message(void* const _uart_raw,
		const unsigned int count, const control_parameter_t* parameters);

void device_uart_raw_init(device_uart_raw_t* const uart_raw,
		uart_light_regs_t* const uart_light, const int id) {
	datastream_source_init(&uart_raw->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	uart_raw->super.send_data = device_uart_raw_send_data;

	uart_raw->data_out = &data_port_dummy;

	uart_raw->control_in = control_port_dummy;
	uart_raw->control_in.parent = (void*) uart_raw;
	uart_raw->control_in.new_control_message = device_uart_raw_new_control_message;

	uart_raw->uart_light = uart_light;

	uart_light_enable_rxint(uart_light);
}

control_port_t* device_uart_raw_get_control_in(device_uart_raw_t* const uart_raw) {
	return &uart_raw->control_in;
}

void device_uart_raw_set_data_out(device_uart_raw_t* const uart_raw,
		const data_port_t* const data_in) {
	uart_raw->data_out = data_in;
}

static void device_uart_raw_send_data(void* const _uart_raw,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_uart_raw_t* uart_raw = (device_uart_raw_t*) _uart_raw;

	unsigned char byte;
	if (uart_light_receive_nb(uart_raw->uart_light, &byte) == UART_OK) {
		data_package_t package = { id, "byte", DATA_TYPE_BYTE, &byte, timestamp };
		uart_raw->data_out->new_data(uart_raw->data_out->parent, &package);
	}
}

static void device_uart_raw_new_control_message(void* const _uart_raw,
		unsigned int count, const control_parameter_t* parameters) {  //TODO remove
	while (count--)
		uart_light_send(((device_uart_raw_t*) _uart_raw)->uart_light,
				(parameters++)->type);
}
