/**
 * @file device_max31855.h
 * @brief MAX31855 thermocouple-to-digital device driver
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_MAX31855_H_
#define DEVICE_MAX31855_H_

#include <fpga-log/datastream_source.h>

#include <fpga-log/control_port.h>
#include <fpga-log/data_port.h>
#include <peripherals/spi_master.h>

#define MAX31855_MAX_SPI_CLK 5000000
#define MAX31855_THERMOCOUPLE_TEMP_NAME "thermocouple temperature"
#define MAX31855_INTERNAL_TEMP_NAME "internal temperature"
#define MAX31855_FAULT_NAMES { "SCV Fault", "SCG Fault", "OC Fault" }

/**
 * @brief enumeration of the different control parameters of the MAX31855 device
 *
 * An enumeration is used for this single value to let the config-tool correctly recognize it as parameter type.
 */
typedef enum {
	MAX31855_PARAMETER_READ = 'r', /**< read data from MAX31855 device */
} max31855_parameter_cpt;

/**
 * @brief struct describing a max31855 device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* data_out; /**< data output destination */
	const data_port_t* error_out; /**< data output destination */
	const data_port_t* internal_temp_out; /**< data output destination */
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */

	spi_master_regs_t* spi_master; /**< pointer to SPI-master hardware registers */
} device_max31855_t;

/**
 * @brief max31855 device init function
 *
 * Initializes the max31855 device, should be called before using the device.
 *
 * @param max31855		pointer to the max31855 device
 * @param spi_master	pointer to a i2c-master peripheral
 * @param id					id used for timestamps
 */
void device_max31855_init(device_max31855_t* const max31855,
		spi_master_regs_t* const spi_master, const int id);

/**
 * @brief returns the max31855 device control input
 *
 * @param max31855		pointer to the max31855 device
 * @return the control input port
 */
control_port_t* device_max31855_get_control_in(
		device_max31855_t* const max31855);

/**
 * @brief connects the data output port of a max31855 device to a given destination
 *
 * @param	max31855	pointer to the max31855 device
 * @param	data_in		the new data destination
 */
void device_max31855_set_data_out(device_max31855_t* const max31855,
		const data_port_t* const data_in);

/**
 * @brief connects the internal temperature output port of a max31855 device to a given destination
 *
 * @param	max31855	pointer to the max31855 device
 * @param	data_in		the new data destination
 */
void device_max31855_set_internal_temp_out(device_max31855_t* const max31855,
		const data_port_t* const data_in);

/**
 * @brief connects the error output port of a max31855 device to a given destination
 *
 * @param	max31855	pointer to the max31855 device
 * @param	data_in		the new data destination
 */
void device_max31855_set_error_out(device_max31855_t* const max31855,
		const data_port_t* const data_in);

#endif
