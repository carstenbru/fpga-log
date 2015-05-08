/**
 * @file device_vl6180.h
 * @brief VL6180 proximity and ambient light sensor
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_VL6180_H_
#define DEVICE_VL6180_H_

#include <fpga-log/datastream_source.h>

#include <fpga-log/control_port.h>
#include <fpga-log/data_port.h>
#include <peripherals/i2c_master.h>

#define VL6180_ADDRESS 82
#define VL6180_I2C_CLOCK 400000
#define VL6180_I2C_RETRIES 3

/**
 * @brief enumeration of the different control parameters of the VL6180 device
 */
typedef enum {
	VL6180_PARAMETER_START_RANGE = 'r', /**< starts a single shot range measurement */
	VL6180_PARAMETER_START_ALS = 'a', /**< starts a single shot ALS (ambient light) measurement */
//TODO
} vl6180_parameter_cpt;

typedef enum {
	VL6180_REGISTER_SYSTEM__MODE_GPIO1 = 0x11,
	VL6180_REGISTER_SYSTEM__INTERRUPT_CONFIG_GPIO = 0x14,
	VL6180_REGISTER_SYSTEM__INTERRUPT_CLEAR = 0x15,
	VL6180_REGISTER_SYSRANGE__START = 0x18,
	VL6180_REGISTER_SYSRANGE__INTERMEASUREMENT_PERIOD = 0x1B,
	VL6180_REGISTER_SYSALS__START = 0x38,
	VL6180_REGISTER_SYSALS__INTERMEASUREMENT_PERIOD = 0x3E,
	VL6180_REGISTER_SYSALS__INTEGRATION_PERIOD = 0x40,
	VL6180_REGISTER_RESULT__RANGE_STATUS = 0x4D,
	VL6180_REGISTER_RESULT__ALS_VAL = 0x50,
	VL6180_REGISTER_RESULT__RANGE_VAL = 0x62,
} vl6180_registers;

/**
 * @brief struct describing a vl6180 device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* range_out; /**< range output destination */
	const data_port_t* als_out; /**< ALS (ambient light sensor) output destination */
	const data_port_t* error_out; /**< error output destination */
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */

	i2c_master_regs_t* i2c_master; /**< pointer to I2C-master hardware registers */
} device_vl6180_t;

/**
 * @brief vl6180 device init function
 *
 * Initializes the vl6180 device, should be called before using the device.
 *
 * @param vl6180			pointer to the vl6180 device
 * @param i2c_master	pointer to a i2c-master peripheral
 * @param	int_id			id of the interrupt pin in the timestamp generator
 */
void device_vl6180_init(device_vl6180_t* const vl6180,
		i2c_master_regs_t* const i2c_master, const int int_id);

/**
 * @brief returns the vl6180 device control input
 *
 * @param vl6180		pointer to the vl6180 device
 * @return the control input port
 */
control_port_t* device_vl6180_get_control_in(device_vl6180_t* const vl6180);

/**
 * @brief connects the range output port of a vl6180 device to a given destination
 *
 * @param	vl6180		pointer to the ads1115 device
 * @param	data_in		the new data destination
 */
void device_vl6180_set_range_out(device_vl6180_t* const vl6180,
		const data_port_t* const data_in);

/**
 * @brief connects the ALS (ambient light sensor) output port of a vl6180 device to a given destination
 *
 * @param	vl6180		pointer to the ads1115 device
 * @param	data_in		the new data destination
 */
void device_vl6180_set_als_out(device_vl6180_t* const vl6180,
		const data_port_t* const data_in);

/**
 * @brief connects the error output port of a vl6180 device to a given destination
 *
 * @param	vl6180		pointer to the vl6180 device
 * @param	data_in		the new data destination
 */
void device_vl6180_set_error_out(device_vl6180_t* const vl6180,
		const data_port_t* const data_in);

/**
 * @brief starts continuous range measurements with a period of period_ms
 *
 * @param vl6180		pointer to the vl6180 device
 * @param period_ms	period in ms, however the real period will be rounded down to the next 10ms (e.g. 357ms will be 350ms), minimum value: 10ms
 */
void device_vl6180_start_range_continous(device_vl6180_t* const vl6180,
		unsigned int period_ms);

/**
 * @brief starts continuous ALS (ambient light) measurements with a period of period_ms
 *
 * @param vl6180		pointer to the vl6180 device
 * @param period_ms	period in ms, however the real period will be rounded down to the next 10ms (e.g. 357ms will be 350ms), minimum value: 10ms
 */
void device_vl6180_start_als_continous(device_vl6180_t* const vl6180,
		unsigned int period_ms);

/**
 * @brief starts continuous both (range+ALS) measurements with a period of period_ms
 *
 * @param vl6180		pointer to the vl6180 device
 * @param period_ms	period in ms, however the real period will be rounded down to the next 10ms (e.g. 357ms will be 350ms), minimum value: 10ms
 */
void device_vl6180_start_both_continous(device_vl6180_t* const vl6180,
		unsigned int period_ms);

#endif
