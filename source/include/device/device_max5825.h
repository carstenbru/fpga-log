/**
 * @file device_max5825.h
 * @brief MAX5825 12-bit 8-channel DAC device driver
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_MAX5825_H_
#define DEVICE_MAX5825_H_

#include <fpga-log/datastream_source.h>

#include <fpga-log/control_port.h>
#include <fpga-log/data_port.h>
#include <peripherals/i2c_master.h>

#define MAX5825_I2C_CLOCK 400000
#define MAX5825_I2C_DEFAULT_ADDRESS 0
#define MAX5825_I2C_RETRIES 3

#define MAX5825_NOT_RESPONDING "MAX5825 not responding"

/**
 * @brief enumeration of the different control parameters of the MAX5825 device
 */
typedef enum {
	MAX5825_PARAMETER_CHANNEL = 'c', /**< DAC channel selection */
	MAX5825_PARAMETER_VALUE = 'v', /**< DAC value (written to CODE register) */
	MAX5825_PARAMETER_UPDATE = 'u', /**< if present a LOAD operation is performed */
	MAX5825_PARAMETER_RETURN_VALUE = 'r', /**< RETURN value of hte DAC */
} max5825_parameter_cpt;

typedef enum {
	MAX5825_REF_EXT = 0, /**< external reference */
	MAX5825_REF_2_048V = 2, /**< internal reference 2.048V */
	MAX5825_REF_2_500V = 1, /**< internal reference 2.500V */
	MAX5825_REF_4_096V = 3 /**< internal reference 4.096V */
} max5825_reference_setting;

typedef enum {
	MAX5825_PW_NORMAL = 0, /**< active: normal power mode */
	MAX5825_PW_1K = 1, /**< power down: 1kOhm termination */
	MAX5825_PW_100K = 2, /**< power down: 100kOhm termination */
	MAX5825_PW_HI_Z = 3 /**< power down: high impedance */
} max5825_power_mode;

typedef enum {
	MAX5825_DEFAULT_MZ = 0,
	MAX5825_DEFAULT_ZERO = 1,
	MAX5825_DEFAULT_MID = 2,
	MAX5825_DEFAULT_FULL = 3,
	MAX5825_DEFAULT_RETURN = 4,
} max5825_default_value;

typedef enum {
	MAX5825_WD_DIS = 0,
	MAX5825_WD_GATE = 1,
	MAX5825_WD_CLR = 2,
	MAX5825_WD_HOLD = 3,
} max5825_wdog_config;

typedef enum {
	MAX5825_GATE_ENABLE = 0, MAX5825_GATE_DISABLE = 1,
} max5825_gate_enable;

typedef enum {
	MAX5825_LATCH_ENABLE = 0, /**<  DAC latch is operational. enabling LDAC and LOAD functions */
	MAX5825_LATCH_DISABLE = 1, /**< DAC latch is transparent. the CODE register content controls the DAC output directly */
} max5825_load_enable;

typedef enum {
	MAX5825_CLEAR_ENABLE = 0, /**<  Clear input and command functions impact the DAC. clearing CODE and DAC registers to their DEFAULT value */
	MAX5825_CLEAR_DISABLE = 1, /**< Clear input and command functions have no effect on the DAC */
} max5825_clear_enable;

typedef enum {
	MAX5825_DAC0 = 0,
	MAX5825_DAC1 = 1,
	MAX5825_DAC2 = 2,
	MAX5825_DAC3 = 3,
	MAX5825_DAC4 = 4,
	MAX5825_DAC5 = 5,
	MAX5825_DAC6 = 6,
	MAX5825_DAC7 = 7,
	MAX5825_DAC_ALL = 8,
} max5825_selection;

/**
 * @brief struct describing a MAX5825 device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t */

	const data_port_t* error_out; /**< error output destination */
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */

	i2c_master_regs_t* i2c_master; /**< pointer to I2C-master hardware registers */

	unsigned char i2c_address; /**< address of the DAC at the I2C-Bus */
	char error_command;
} device_max5825_t;

void device_max5825_init(device_max5825_t* const max5825,
		i2c_master_regs_t* const i2c_master, const unsigned int id,
		max5825_reference_setting reference_setting);

control_port_t* device_max5825_get_control_in(device_max5825_t* const max5825);

void device_max5825_set_error_out(device_max5825_t* const max5825,
		const data_port_t* const data_in);

/**
 * @brief sets the I2C address of the DAC, only last four bits are required since the rest is fixed
 *
 * @param max5825			pointer to the max5825 device
 * @param i2c_address	the new address
 */
void device_max5825_set_i2c_address(device_max5825_t* const max5825,
		const unsigned char i2c_address);

/**
 * @brief sets the power mode of DAC channels
 *
 * @param max5825			pointer to the max5825 device
 * @param dac_mask		mask of DACs for which the new setting should be applied (one bit per DAC)
 * @param power_mode	the new power mode
 */
void device_max5825_set_power_mode(device_max5825_t* const max5825,
		const unsigned char dac_mask, max5825_power_mode power_mode);

/**
 * @brief sets the default value of DAC channels
 *
 * @param max5825				pointer to the max5825 device
 * @param dac_mask			mask of DACs for which the new setting should be applied (one bit per DAC)
 * @param default_value	the new default value
 */
void device_max5825_set_default(device_max5825_t* const max5825,
		const unsigned char dac_mask, max5825_default_value default_value);

void device_max5825_set_config(device_max5825_t* const max5825,
		const unsigned char dac_mask, max5825_wdog_config wdog,
		max5825_gate_enable sw_gate, max5825_load_enable load_enable,
		max5825_clear_enable clear_enable);

/**
 * @brief loads the data from a CODE register to an output latch
 *
 * @param max5825		pointer to the max5825 device
 * @param dac_sel		DAC selection
 */
void device_max5825_load(device_max5825_t* const max5825,
		max5825_selection dac_sel);

/**
 * @brief sets the value of a RETURN register
 *
 * @param max5825		pointer to the max5825 device
 * @param dac_sel		DAC selection
 * @param value			the new value
 */
void device_max5825_write_return(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value);

/**
 * @brief sets the value of a CODE register
 *
 * @param max5825		pointer to the max5825 device
 * @param dac_sel		DAC selection
 * @param value			the new value
 */
void device_max5825_write_code(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value);

/**
 * @brief sets the value of a CODE register and updates an DAC output
 *
 * @param max5825		pointer to the max5825 device
 * @param dac_sel		DAC selection
 * @param value			the new value
 */
void device_max5825_write_code_load(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value);

/**
 * @brief sets the value of a CODE register and updates all DAC outputs
 *
 * @param max5825		pointer to the max5825 device
 * @param dac_sel		DAC selection
 * @param value			the new value
 */
void device_max5825_write_code_load_all(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value);

#endif
