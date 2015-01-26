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
#include <fpga-log/simple_float.h>

#define NMEA_RMC_NAMES { "time", "status", "latitude_rmc", "N/S" , "longitude_rmc", "E/W", "speed", "course", "date", "magnetic variation", "variation E/W", "mode"}
#define NMEA_GGA_NAMES { "time", "latitude", "N/S" , "longitude", "E/W", "fix indicator", "satellites", "HDOP", "altitude", "units", "geoid seperation", "units 2", "age of diff. corr.", "ref. station ID"}
#define NMEA_VTG_NAMES { "course", "reference", "course" , "reference", "speed", "units", "speed", "units", "mode"}
#define NMEA_UNKNOWN "unknown"

typedef enum {
	GPS_SYNC_DISABLE = 0, /**< no synchronization */
	GPS_SYNC_TIME = 1 /**< synchronize data logger time to GPS time and not date **/
	//TODO add sync mode with date also used for logger time
} device_gps_sync_mode;

/** 
 * @brief struct describing a gps device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* raw_data_out; /**< raw data output destination */
	const data_port_t* parsed_data_out; /**< parsed data output destination */

	device_gps_sync_mode sync_logger_time; /**< sync mode */

	unsigned char parse_status;
	unsigned char cur_sentence[3]; /**< current received NMEA sentence type */
	data_package_t parse_package; /**< currently parsed package */
	unsigned int parse_uint;
	unsigned char parse_byte;
	unsigned char parsed_digits;
	simple_float_b10_t parse_float;

	unsigned char time_parse_position;
	unsigned int time_parsed;
	unsigned int time_parsed_sec;

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
		uart_light_regs_t* const uart_light, device_gps_sync_mode sync_logger_time, const int id);

/**
 * @brief connects the raw data output port of a gps device to a given destination
 * 
 * @param	gps_nmea	pointer to the gps device
 * @param	data_in		the new data destination
 */
void device_gps_nmea_set_raw_data_out(device_gps_nmea_t* const gps_nmea,
		const data_port_t* const data_in);

/**
 * @brief connects the parsed data output port of a gps device to a given destination
 *
 * @param	gps_nmea	pointer to the gps device
 * @param	data_in		the new data destination
 */
void device_gps_nmea_set_parsed_data_out(device_gps_nmea_t* const gps_nmea,
		const data_port_t* const data_in);

#endif 
