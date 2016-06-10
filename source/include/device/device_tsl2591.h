/**
 * @file device_tsl2591.h
 * @brief TSL2591 two channel ambient light sensor (full spectrum, infrared)
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DEVICE_TSL2591_H_
#define DEVICE_TSL2591_H_

#include <fpga-log/datastream_source.h>

#include <fpga-log/data_port.h>
#include <peripherals/i2c_master.h>

#define TSL2591_ADDRESS (0x29 << 1)
#define TSL2591_I2C_CLOCK 400000
#define TSL2591_I2C_RETRIES 3

#define TSL2591_NORMAL_OP 0b10100000

/**
 * @brief enumeration of the different gain configurations of the TLS2591 device
 */
typedef enum {
	TLS2591_AGAIN_LOW_GAIN = 0b00, /**< low gain mode - gain of 1 */
	TLS2591_AGAIN_MEDIUM_GAIN = 0b01, /**< medium gain mode - gain of 25 */
	TLS2591_AGAIN_HIGH_GAIN = 0b10, /**< high gain mode - gain of 428 */
	TLS2591_AGAIN_MAXIMUM_GAIN = 0b11, /**< maximum gain mode - gain of 9876 */
} tsl2591_again;

/**
 * @brief enumeration of the different integration time configurations of the TLS2591 device
 */
typedef enum {
	TLS2591_AGTIME_100MS = 0b000, /**< 100ms integration time */
	TLS2591_AGTIME_200MS = 0b001, /**< 200ms integration time */
	TLS2591_AGTIME_300MS = 0b010, /**< 300ms integration time */
	TLS2591_AGTIME_400MS = 0b011, /**< 400ms integration time */
	TLS2591_AGTIME_500MS = 0b100, /**< 500ms integration time */
	TLS2591_AGTIME_600MS = 0b101, /**< 600ms integration time */
} tsl2591_atime;

typedef enum {
	TLS2591_REGISTER_ENABLE = 0x00,
	TLS2591_REGISTER_CONFIG = 0x01,
	TLS2591_REGISTER_PERSIST = 0x0C,
	TLS2591_REGISTER_PID = 0x11,
	TLS2591_REGISTER_ID = 0x12,
	TLS2591_REGISTER_STATUS = 0x13,
	TLS2591_REGISTER_C0DATAL = 0x14,
	TLS2591_REGISTER_C0DATAH = 0x15,
	TLS2591_REGISTER_C1DATAL = 0x16,
	TLS2591_REGISTER_C1DATAH = 0x17,
} tls2591_registers;

/**
 * @brief struct describing a tsl2591 device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* ch0_out; /**< ch0 (full-spectrum) output destination */
	const data_port_t* ch1_out; /**< ch1 (infrared) output destination */

	i2c_master_regs_t* i2c_master; /**< pointer to I2C-master hardware registers */
} device_tsl2591_t;

/**
 * @brief tsl2591 device init function
 *
 * Initializes the tsl2591 device, should be called before using the device.
 *
 * @param tsl2591			pointer to the tsl2591 device
 * @param i2c_master	pointer to a i2c-master peripheral
 * @param	int_id			id of the interrupt pin in the timestamp generator
 * @param gain				gain setting of the device
 * @param integration_time	ADC integration time setting
 */
void device_tsl2591_init(device_tsl2591_t* const tsl2591,
		i2c_master_regs_t* const i2c_master, const int int_id, tsl2591_again gain, tsl2591_atime integration_time);

/**
 * @brief connects the ch0 (full-spectrum) output port of a tsl2591 device to a given destination
 *
 * @param	tsl2591		pointer to the tsl2591 device
 * @param	data_in		the new data destination
 */
void device_tsl2591_set_ch0_out(device_tsl2591_t* const tsl2591,
		const data_port_t* const data_in);

/**
 * @brief connects the ch1 (infrared) output port of a tsl2591 device to a given destination
 *
 * @param	tsl2591		pointer to the tsl2591 device
 * @param	data_in		the new data destination
 */
void device_tsl2591_set_ch1_out(device_tsl2591_t* const tsl2591,
		const data_port_t* const data_in);

#endif
