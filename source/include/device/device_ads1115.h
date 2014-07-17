/**
 * @file device_ads1115.h
 * @brief ADS1115 16-bit 4-channel (single-ended, alternatively 2-channel differential) ADC device driver
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_ADS1115_H_
#define DEVICE_ADS1115_H_

#include <fpga-log/datastream_source.h>

#include <fpga-log/control_port.h>
#include <fpga-log/data_port.h>
#include <peripherals/i2c_master.h>

#define ADS1115_ADDRESS 144
#define ADS1115_DEFAULT_CONFIG 0
#define ADS1115_I2C_CLOCK 400000
#define ADS1115_I2C_RETRIES 3

/**
 * @brief enumeration of the different input multiplexer configurations of the ADS1115 device
 */
typedef enum {
	ADS1115_MUX_DIFF_CH0_CH1 = 0b000, /**< differential input between AIN0 and AIN1 */
	ADS1115_MUX_DIFF_CH0_CH3 = 0b001, /**< differential input between AIN0 and AIN3 */
	ADS1115_MUX_DIFF_CH1_CH3 = 0b010, /**< differential input between AIN1 and AIN3 */
	ADS1115_MUX_DIFF_CH2_CH3 = 0b011, /**< differential input between AIN2 and AIN3 */
	ADS1115_MUX_SE_CH0 = 0b100, /**< single-ended input AIN0 */
	ADS1115_MUX_SE_CH1 = 0b101, /**< single-ended input AIN1 */
	ADS1115_MUX_SE_CH2 = 0b110, /**< single-ended input AIN2 */
	ADS1115_MUX_SE_CH3 = 0b111 /**< single-ended input AIN3 */
} ads1115_mux;

#define ADS1115_MUX_MASK 0b0111000000000000
#define ADS1115_MUX_SHIFT 12

#define ADS1115_CHANNEL_NAMES { "diff ch0-ch1", "diff ch0-ch3", "diff ch1-c3", "diff ch2-ch3", "ch0", "ch1", "ch2", "ch3" }

/**
 * @brief enumeration of the different gain configurations of the ADS1115 device
 */
typedef enum {
	ADS1115_PGA_6_144V = 0b000, /**< full-scale voltage: 6.144V */
	ADS1115_PGA_4_096V = 0b001, /**< full-scale voltage: 4.096V */
	ADS1115_PGA_2_048V = 0b010, /**< full-scale voltage: 2.048V */
	ADS1115_PGA_1_024V = 0b011, /**< full-scale voltage: 1.024V */
	ADS1115_PGA_0_512V = 0b100, /**< full-scale voltage: 0.512V */
	ADS1115_PGA_0_256V = 0b101, /**< full-scale voltage: 0.256V */
} ads1115_pga;

#define ADS1115_PGA_MASK 0b0000111000000000
#define ADS1115_PGA_SHIFT 9

/**
 * @brief enumeration of the different modes of the ADS1115 device
 */
typedef enum {
	ADS1115_MODE_CONTINUOUS = 0, /**< continuous conversion mode */
	ADS1115_MODE_SINGLE_SHOT = 1, /**< power-down single shot mode */
} ads1115_mode;

#define ADS1115_MODE_MASK 0b00000100000000
#define ADS1115_MODE_SHIFT 8

/**
 * @brief enumeration of the different data rates of the ADS1115 device
 */
typedef enum {
	ADS1115_DR_8SPS = 0b000, /**< 8 samples per second */
	ADS1115_DR_16SPS = 0b001, /**< 16 samples per second */
	ADS1115_DR_32SPS = 0b010, /**< 32 samples per second */
	ADS1115_DR_64SPS = 0b011, /**< 64 samples per second */
	ADS1115_DR_128SPS = 0b100, /**< 128 samples per second */
	ADS1115_DR_250SPS = 0b101, /**< 250 samples per second */
	ADS1115_DR_475SPS = 0b110, /**< 475 samples per second */
	ADS1115_DR_860SPS = 0b111 /**< 860 samples per second */
} ads1115_data_rate;

#define ADS1115_DATA_RATE_MASK 0b0000011100000
#define ADS1115_DATA_RATE_SHIFT 5

#define ADS1115_CONVERSION_START_SHIFT 15

/**
 * @brief enumeration of the different control parameters of the ADS1115 device
 */
typedef enum {
	ADS1115_PARAMETER_SINGLE_SHOT = 's', /**< starts a single shot measurement */
	ADS1115_PARAMETER_MUX = 'i', /**< input multiplexer configuration */
	ADS1115_PARAMETER_GAIN = 'g', /**< gain configuration */
	ADS1115_PARAMETER_MODE = 'm', /**< mode configuration */
	ADS1115_PARAMETER_DATA_RATE = 'r' /**< data rate configuration */
} ads1115_parameter_cpt;

typedef enum {
	ADS1115_REGISTER_CONVERSION = 0, /**< conversion register which holds measurement results */
	ADS1115_REGISTER_CONFIG = 1, /**< configuration register */
	ADS1115_REGISTER_LO_TRESH = 2, /**< low threshold register (comparator) */
	ADS1115_REGISTER_HI_TRESH = 3 /**< high threshold register (comparator) */
} ads1115_registers;

/**
 * @brief struct describing a ads1115 device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* data_out; /**< data output destination */
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */

	i2c_master_regs_t* i2c_master; /**< pointer to I2C-master hardware registers */

	int config_reg; /**< local configuration register */
	const char* val_name; /**< current channel name */
} device_ads1115_t;

/**
 * @brief ads1115 device init function
 *
 * Initializes the ads1115 device, should be called before using the device.
 *
 * @param ads1115			pointer to the ads1115 device
 * @param i2c_master	pointer to a i2c-master peripheral
 * @param	rdy_id			id of the ready pin in the timestamp generator
 * @param	mux					input multiplexer configuration
 * @param	pga					gain configuration
 * @param mode				device mode configuration
 * @param data_rate		data rate configuration
 */
void device_ads1115_init(device_ads1115_t* const ads1115,
		i2c_master_regs_t* const i2c_master, const int rdy_id, ads1115_mux mux,
		ads1115_pga pga, ads1115_mode mode, ads1115_data_rate data_rate);

/**
 * @brief returns the ads1115 device control input
 *
 * @param ads1115		pointer to the ads1115 device
 * @return the control input port
 */
control_port_t* device_ads1115_get_control_in(device_ads1115_t* const ads1115);

/**
 * @brief connects the data output port of a ads1115 device to a given destination
 *
 * @param	ads1115		pointer to the ads1115 device
 * @param	data_in		the new data destination
 */
void device_ads1115_set_data_out(device_ads1115_t* const ads1115,
		const data_port_t* const data_in);

#endif
