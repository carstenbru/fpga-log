/**
 * @file device_chr_um7.c
 * @brief CHR-UM7 orientation sensor device driver (Binary protocol)
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_CHR_UM7_H_
#define DEVICE_CHR_UM7_H_

#include <uart.h>

#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>

typedef uint9_t uint8_t;
typedef uint18_t uint16_t;
typedef uint_least32_t uint32_t;

typedef enum {
	HEALTH_RATE_OFF = 0, /**< disable health packages */
	HEALTH_RATE_0_125HZ = 1, /**< 0.125 Hz */
	HEALTH_RATE_0_25HZ = 2, /**< 0.25 Hz */
	HEALTH_RATE_0_5HZ = 3, /**< 0.5 Hz */
	HEALTH_RATE_1HZ = 4, /**< 1 Hz */
	HEALTH_RATE_2HZ = 5, /**< 2 Hz */
	HEALTH_RATE_4HZ = 6 /**< 4 Hz */
} device_chr_um7_health_rate;

typedef struct {
	uint8_t address;
	union {
		uint8_t pt;
		struct {
			unsigned int padding :1;
			unsigned int has_data :1;
			unsigned int is_batch :1;
			unsigned int batch_length :4;
			unsigned int hidden :1;
			unsigned int command_failed :1;
		} pt_struct;
	} pt_union;
	uint8_t data_length;
	uint8_t data[15][4];

	uint16_t checksum;
} chr_um7_packet_t;

/**
 * @brief struct describing a CHR-UM7 device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* orientation_data_out; /**< orientation data output destination */
	const data_port_t* proc_data_out; /**< processed data output destination */
	const data_port_t* raw_data_out; /**< raw data output destination */
	const data_port_t* error_out; /**< error output destination */

	int parse_state; /**< current parsing state */
	chr_um7_packet_t um7_packet; /**< last/currently parsing CHR-UM7 packet */

	uart_light_regs_t* uart_light; /**< pointer to UART hardware registers */
} device_chr_um7_t;

/**
 * @brief CHR-UM7 device init function
 *
 * @param	chr_um7		pointer to the CHR-UM7 device
 * @param uart_light 	pointer to a uart_light peripheral
 * @param id		id of the new device (for log output, etc)
 */
void device_chr_um7_init(device_chr_um7_t* const chr_um7,
		uart_light_regs_t* const uart_light, const int id);

/**
 * @brief connects the orientaion data output port of a CHR-UM7 device to a given destination
 *
 * @param	chr_um7		pointer to the CHR-UM7 device
 * @param	data_in		the new data destination
 */
void device_chr_um7_set_orientation_data_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in);

/**
 * @brief connects the processed data output port of a CHR-UM7 device to a given destination
 *
 * @param	chr_um7		pointer to the CHR-UM7 device
 * @param	data_in		the new data destination
 */
void device_chr_um7_set_proc_data_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in);

/**
 * @brief connects the raw data output port of a CHR-UM7 device to a given destination
 *
 * @param	chr_um7		pointer to the CHR-UM7 device
 * @param	data_in		the new data destination
 */
void device_chr_um7_set_raw_data_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in);

/**
 * @brief connects the error output port of a CHR-UM7 device to a given destination
 *
 * @param	chr_um7		pointer to the CHR-UM7 device
 * @param	data_in		the new data destination
 */
void device_chr_um7_set_error_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in);

/**
 * @brief sets the data rates of the different value types for the CHR-UM7 sensor
 *
 * @param chr_um7	pointer to the CHR-UM7 device
 */
void device_chr_um7_set_rates(device_chr_um7_t* const chr_um7,
		uint_least8_t raw_accel_rate, uint_least8_t raw_gyro_rate,
		uint_least8_t raw_mag_rate, uint_least8_t temp_rate,
		uint_least8_t proc_accel_rate, uint_least8_t proc_gyro_rate,
		uint_least8_t proc_mag_rate, uint_least8_t quat_rate,
		uint_least8_t euler_rate, device_chr_um7_health_rate health_rate);

/**
 * @brief issues a zero gyros command, the UM7 device should stay stationary during this calibration
 *
 * @param chr_um7	pointer to the CHR-UM7 device
 */
void device_chr_um7_zero_gyros(device_chr_um7_t* const chr_um7);

#endif 
