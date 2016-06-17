/**
 * @file device_gps_nmea.h
 * @brief GPS receiver device driver (NMEA protocol)
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DEVICE_GPS_NMEA_H_
#define DEVICE_GPS_NMEA_H_

#include <uart.h>

#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>
#include <fpga-log/simple_float.h>
#include <peripherals/timestamp_counter.h>

#define NMEA_RMC_NAMES { "time", "status", "latitude_rmc", "N/S" , "longitude_rmc", "E/W", "speed", "course", "date", "magnetic variation", "variation E/W", "mode"}
#define NMEA_GGA_NAMES { "time", "latitude", "N/S" , "longitude", "E/W", "fix indicator", "satellites", "HDOP", "altitude", "units", "geoid seperation", "units 2", "age of diff. corr.", "ref. station ID"}
#define NMEA_VTG_NAMES { "course", "reference", "course" , "reference", "speed", "units", "speed", "units", "mode"}
#define NMEA_UNKNOWN "unknown"

#define DEVICE_GPS_NMEA_DEFAULT_TIMESTAMP_MISS_ASSUMPTION 4
#define DEVICE_GPS_NMEA_DEFAULT_MAY_BYTES_PER_CALL 8

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

	unsigned int timestamp_miss_counter;
	unsigned int timestamp_miss_assumption;

	unsigned int max_bytes_per_call;

	uart_light_regs_t* uart_light; /**< pointer to UART hardware registers */
	timestamp_counter_regs_t* timestamp_counter; /**< pointer to timestamp counter peripheral (to update system time) */
} device_gps_nmea_t;

/**
 * @brief gps device init function
 * 
 * @param	gps_nmea	pointer to the gps device
 * @param uart_light 	pointer to a uart_light peripheral
 * @param id		id of the new device (for log output, etc)
 */
void device_gps_nmea_init(device_gps_nmea_t* const gps_nmea,
		uart_light_regs_t* const uart_light, device_gps_sync_mode sync_logger_time, timestamp_counter_regs_t* timestamp_counter, const int id);

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

/**
 * @brief sets the maximum bytes processed by the module at once, if you have performance problems in general decrease this value, if you have problems in the GPS module increase it
 *
 * After this amount of bytes control is given back to the datalogger so that other modules can work. The remaining bytes are
 * processed in the next call.
 * default: 8
 *
 * @param gps_nmea	pointer to the gps device
 * @param max_bytes_per_call the maximum bytes processed by the module at once
 */
void device_gps_nmea_set_max_bytes_per_call(device_gps_nmea_t* const gps_nmea, unsigned int max_bytes_per_call);

/**
 * @brief sets the timestamp miss assumption value, if you have performance problems increase this value, if you have trouble that the device stops working decrease it
 *
 * The timestamp miss assumption value is the number of calls of the "update" function with data present in the UART fifo until
 * a missing timestamp is assumed (which could happen e.g. at start-up)
 * default: 4
 *
 * @param gps_nmea	pointer to the gps device
 * @param timestamp_miss_assumption the timestamp miss assumption value
 */
void device_gps_nmea_set_timestamp_miss_assumption(device_gps_nmea_t* const gps_nmea, unsigned int timestamp_miss_assumption);

#endif 
