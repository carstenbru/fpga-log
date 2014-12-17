/**
 * @file device_gps_nmea.c
 * @brief GPS receiver device driver (NMEA protocol)
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_gps_nmea.h>
#include <fpga-log/peripheral_funcs/uart_light_funcs.h>

/**
 * @brief gps device send data function
 *
 * @param	_gps_nmea		pointer to the gps device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_gps_nmea_send_data(void* const _gps_nmea,
		const unsigned int id, const timestamp_t* const timestamp);

void device_gps_nmea_init(device_gps_nmea_t* const gps_nmea,
		uart_light_regs_t* const uart_light, const int id) {
	datastream_source_init(&gps_nmea->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	gps_nmea->super.send_data = device_gps_nmea_send_data;

	gps_nmea->raw_data_out = &data_port_dummy;

	gps_nmea->uart_light = uart_light;

	uart_light_enable_rxint(uart_light);
}

void device_gps_nmea_set_raw_data_out(device_gps_nmea_t* const gps_nmea,
		const data_port_t* const data_in) {
	gps_nmea->raw_data_out = data_in;
}

static void device_gps_nmea_send_data(void* const _gps_nmea,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_gps_nmea_t* gps_nmea = (device_gps_nmea_t*) _gps_nmea;

	unsigned char byte;
	if (uart_light_receive_nb(gps_nmea->uart_light, &byte) == UART_OK) {
		data_package_t package = { id, "byte", DATA_TYPE_BYTE, &byte, timestamp };
		gps_nmea->raw_data_out->new_data(gps_nmea->raw_data_out->parent, &package);
	}
}
