/**
 * @file device_gps_nmea.h
 * @brief GPS receiver device driver (NMEA protocol)
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_GPS_NMEA_H_
#define DEVICE_GPS_NMEA_H_

#include <uart.h>

#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>

/** 
 * @brief struct describing a gps device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* raw_data_out; /**< raw data output destination */

	uart_light_regs_t* uart_light; /**< pointer to UART hardware registers */
} device_gps_nmea_t;

/**
 * @brief gps device init function
 * 
 * @param	gps_nmea	pointer to the gps device
 * @param uart_light 	pointer to a uart_light peripheral
 * @param id		id of the new device (for log output, etc)
 */
void device_gps_nmea_init(device_gps_nmea_t* const gps_nmea,
		uart_light_regs_t* const uart_light, const int id);

/**
 * @brief connects the raw data output port of a gps device to a given destination
 * 
 * @param	gps_nmea	pointer to the gps device
 * @param	data_in		the new data destination
 */
void device_gps_nmea_set_raw_data_out(device_gps_nmea_t* const gps_nmea,
		const data_port_t* const data_in);

#endif 
