/**
 * @file device_ads1115.c
 * @brief ADS1115 16-bit 4-channel (single-ended, alternatively 2-channel differential) ADC device driver
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/device/device_ads1115.h>
#include <fpga-log/peripheral_funcs/i2c_funcs.h>
#include <fpga-log/sys_init.h>

const char* ads1115_channel_names[8] = ADS1115_CHANNEL_NAMES;

/**
 * @brief ads1115 send data function
 *
 * This function sends the data packages to the devices output port.
 *
 * @param	_ads1115		pointer to the ads1115 device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_ads1115_send_data(void* const _ads1115,
		const unsigned int id, const timestamp_t* const timestamp);

/**
 * @brief control message function of ads1115 device
 *
 * @param	parent			the ads1115 device
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void device_ads1115_control_message(void* const _ads1115,
		unsigned int count, const control_parameter_t* parameters);

/**
 * @brief sets a value in the (local) configuration register
 *
 * @param ads1115		the ads1115 device
 * @param parameter	type of the value to set
 * @param value			the value to set
 */
static void device_ads1115_set_config_value(device_ads1115_t* const ads1115,
		ads1115_parameter_cpt parameter, int value);

/**
 * @brief send the local configuration register value to the ADS1115
 *
 * After sending is completed the pointer register of the ADS1115 is set to the conversion register
 * to enable simple readings of conversion results.
 *
 * @param ads1115		the ads1115 device
 */
static void device_ads1115_send_configuration_register(
		device_ads1115_t* const ads1115);

/**
 * @brief update function of the ads1115 device
 *
 * @param _ads1115	pointer to the ads1115 device
 */
static void device_ads1115_update(void* const ads1115);

/**
 * @brief sets the bits of reg which are one in mask to the bits in value
 *
 * @param reg		value in which bits should be modified
 * @param mask	mask of bits to modify
 * @param value	new value of the bits
 */
static void set_bits(int* reg, int mask, int value);

void device_ads1115_init(device_ads1115_t* const ads1115,
		i2c_master_regs_t* const i2c_master, const int rdy_id, ads1115_mux mux,
		ads1115_pga pga, ads1115_mode mode, ads1115_data_rate data_rate) {
	datastream_source_init(&ads1115->super, rdy_id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	ads1115->super.super.update = device_ads1115_update;
	ads1115->super.send_data = device_ads1115_send_data;

	ads1115->i2c_master = i2c_master;

	ads1115->control_in = control_port_dummy;
	ads1115->control_in.parent = (void*) ads1115;
	ads1115->control_in.new_control_message = device_ads1115_control_message;

	ads1115->data_out = &data_port_dummy;

	//from datasheet:
	//prescaler = (peripheral_clock / (5 * desired_SCL))-1
	unsigned long int prescaler = get_peri_clock();
	prescaler /= (5 * ADS1115_I2C_CLOCK);
	prescaler--;
	i2c_init(i2c_master, prescaler);

	ads1115->config_reg = ADS1115_DEFAULT_CONFIG;
	device_ads1115_set_config_value(ads1115, ADS1115_PARAMETER_MUX, mux);
	device_ads1115_set_config_value(ads1115, ADS1115_PARAMETER_GAIN, pga);
	device_ads1115_set_config_value(ads1115, ADS1115_PARAMETER_MODE, mode);
	device_ads1115_set_config_value(ads1115, ADS1115_PARAMETER_DATA_RATE,
			data_rate);

	//configure ALERT/RDY pin as result ready pin like mentioned on page 20 of ADS1115 datasheet
	unsigned char data_lo[3] = { ADS1115_REGISTER_LO_TRESH, 0, 0 };
	i2c_action_w_retry(i2c_master, ADS1115_ADDRESS, 3, data_lo, i2c_write,
	ADS1115_I2C_RETRIES);
	unsigned char data_hi[3] = { ADS1115_REGISTER_HI_TRESH, 128, 0 };
	i2c_action_w_retry(i2c_master, ADS1115_ADDRESS, 3, data_hi, i2c_write,
	ADS1115_I2C_RETRIES);

	device_ads1115_send_configuration_register(ads1115);
}

control_port_t* device_ads1115_get_control_in(device_ads1115_t* const ads1115) {
	return &ads1115->control_in;
}

void device_ads1115_set_data_out(device_ads1115_t* const ads1115,
		const data_port_t* const data_in) {
	ads1115->data_out = data_in;
}

static void set_bits(int* reg, int mask, int value) {
	*reg &= ~mask;
	*reg |= (value & mask);
}

static void device_ads1115_set_config_value(device_ads1115_t* const ads1115,
		ads1115_parameter_cpt parameter, int value) {
	switch (parameter) {
	case ADS1115_PARAMETER_MUX:
		set_bits(&ads1115->config_reg, ADS1115_MUX_MASK,
				value << ADS1115_MUX_SHIFT);
		ads1115->val_name = ads1115_channel_names[value];
		break;
	case ADS1115_PARAMETER_GAIN:
		set_bits(&ads1115->config_reg, ADS1115_PGA_MASK,
				value << ADS1115_PGA_SHIFT);
		break;
	case ADS1115_PARAMETER_MODE:
		set_bits(&ads1115->config_reg, ADS1115_MODE_MASK,
				value << ADS1115_MODE_SHIFT);
		break;
	case ADS1115_PARAMETER_DATA_RATE:
		set_bits(&ads1115->config_reg, ADS1115_DATA_RATE_MASK,
				value << ADS1115_DATA_RATE_SHIFT);
		break;
	case ADS1115_PARAMETER_SINGLE_SHOT:
		ads1115->config_reg |= (1 << ADS1115_CONVERSION_START_SHIFT);
		break;
	}
}

static void device_ads1115_send_configuration_register(
		device_ads1115_t* const ads1115) {
	unsigned char data_config[3] = { ADS1115_REGISTER_CONFIG, ads1115->config_reg
			>> 8, ads1115->config_reg };
	i2c_action_w_retry(ads1115->i2c_master, ADS1115_ADDRESS, 3, data_config,
			i2c_write, ADS1115_I2C_RETRIES);

	//set pointer register to conversion register for faster result reading
	unsigned char pointer_reg = ADS1115_REGISTER_CONVERSION;
	i2c_action_w_retry(ads1115->i2c_master, ADS1115_ADDRESS, 1, &pointer_reg,
			i2c_write, ADS1115_I2C_RETRIES);

	//reset single conversion start bit
	ads1115->config_reg &= ~(1 << ADS1115_CONVERSION_START_SHIFT);
}

static void device_ads1115_control_message(void* const _ads1115,
		unsigned int count, const control_parameter_t* parameters) {
	device_ads1115_t* ads1115 = (device_ads1115_t*) _ads1115;

	while (count--) {
		device_ads1115_set_config_value(ads1115, parameters->type,
				parameters->value);
		parameters++;
	}
	device_ads1115_send_configuration_register(ads1115);
}

static void device_ads1115_update(void* const ads1115) {
}

static void device_ads1115_send_data(void* const _ads1115,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_ads1115_t* ads1115 = (device_ads1115_t*) _ads1115;

	unsigned char data[2];
	i2c_read(ads1115->i2c_master, ADS1115_ADDRESS, 2, data);
	int value = (data[0] << 8) + data[1];
	if ((value & 32767) != value)
		value -= 65536;  //convert 16.bit k2-komplement
	data_package_t package = { id, ads1115->val_name, DATA_TYPE_INT, &value,
			timestamp };
	ads1115->data_out->new_data(ads1115->data_out->parent, &package);
}
