/**
 * @file device_uart_raw.c
 * @brief simple raw uart logging device driver
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "device/device_uart_raw.h"

/**
 * @brief uart raw update function
 *
 * everything that does not need to be done immediately in an interrupt is done here, e.g. sending data to the data_out port
 *
 * @param	_uart_raw		pointer to the uart raw device
 */
static void device_uart_raw_update(void* const _uart_raw);

/**
 * @brief measure function of uart raw
 *
 * @param	parent		the uart_raw device
 */
static void device_uart_raw_measure(void* const uart_raw);

void device_uart_raw_init(device_uart_raw_t* const uart_raw,
		uart_light_regs_t* const uart_light, const int id) {
	datastream_object_init(&uart_raw->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	uart_raw->super.update = device_uart_raw_update;

	uart_raw->data_out = &data_port_dummy;

	uart_raw->control_in = control_port_dummy;
	uart_raw->control_in.parent = (void*) uart_raw;
	uart_raw->control_in.measure = device_uart_raw_measure;
	//TODO set function pointers of control_in here

	uart_raw->uart_light = uart_light;
}

void device_uart_raw_set_data_out(device_uart_raw_t* const uart_raw,
		const data_port_t* const data_in) {
	uart_raw->data_out = data_in;
}

static void device_uart_raw_update(void* const _uart_raw) {
	device_uart_raw_t* uart_raw = (device_uart_raw_t*) _uart_raw;

	unsigned char byte;
	if (uart_light_receive_nb(uart_raw->uart_light, &byte) == UART_OK) {
		data_package_t package = { uart_raw->id, DATA_TYPE_BYTE, &byte };
		uart_raw->data_out->new_data(uart_raw->data_out->parent, &package);
	}
}

static void device_uart_raw_measure(void* const uart_raw) {  //TODO remove
	uart_light_send(((device_uart_raw_t*)uart_raw)->uart_light, 'm');
}
