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

typedef struct {
	uint8_t address;
	union {
		uint8_t pt;
		struct { //TODO check if right order/alignment
			unsigned int padding :1;
			unsigned int has_data :1;
			unsigned int is_batch :1;
			unsigned int batch_length :4;
			unsigned int hidden :1;
			unsigned int command_failed :1;
		};
	};
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

#endif 
