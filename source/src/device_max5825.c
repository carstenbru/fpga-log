/**
 * @file device_max5825.c
 * @brief MAX5825 12-bit 8-channel DAC device driver
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_max5825.h>
#include <fpga-log/peripheral_funcs/i2c_funcs.h>
#include <fpga-log/sys_init.h>

//TODO readback/verification possibilities
//TODO watchdog features?

unsigned char* max5825_not_responding = MAX5825_NOT_RESPONDING;

static void device_max5825_send_data(void* const _max5825,
		const unsigned int id, const timestamp_t* const timestamp);

static void device_max5825_control_message(void* const _max5825,
		unsigned int count, const control_parameter_t* parameters);

static void device_max5825_update(void* const max5825);

static void device_max5825_set_reference(device_max5825_t* const max5825,
		max5825_reference_setting reference_setting);

/**
 * @brief sends a command to the max5825
 *
 * @param max5825				pointer to the max5825 device
 * @param command				pointer to an array holding the three bytes to send
 */
static void device_max5825_send_command(device_max5825_t* const max5825,
		unsigned char* command);

void device_max5825_init(device_max5825_t* const max5825,
		i2c_master_regs_t* const i2c_master, const unsigned int id,
		max5825_reference_setting reference_setting) {
	datastream_source_init(&max5825->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	max5825->super.super.update = device_max5825_update;
	max5825->super.send_data = device_max5825_send_data;

	max5825->i2c_master = i2c_master;

	max5825->control_in = control_port_dummy;
	max5825->control_in.parent = (void*) max5825;
	max5825->control_in.new_control_message = device_max5825_control_message;

	max5825->error_out = &data_port_dummy;

	device_max5825_set_i2c_address(max5825, MAX5825_I2C_DEFAULT_ADDRESS);

	//from datasheet:
	//prescaler = (peripheral_clock / (5 * desired_SCL))-1
	unsigned long int prescaler = get_peri_clock();
	prescaler /= (5 * MAX5825_I2C_CLOCK);
	prescaler--;
	i2c_init(i2c_master, prescaler);

	device_max5825_set_reference(max5825, reference_setting);
}

control_port_t* device_max5825_get_control_in(device_max5825_t* const max5825) {
	return &max5825->control_in;
}

void device_max5825_set_error_out(device_max5825_t* const max5825,
		const data_port_t* const data_in) {
	max5825->error_out = data_in;
}

static void device_max5825_control_message(void* const _max5825,
		unsigned int count, const control_parameter_t* parameters) {
	device_max5825_t* max5825 = (device_max5825_t*) _max5825;

	unsigned int channels = MAX5825_DAC_ALL;  //all DACs
	int value = -1;
	int return_val = -1;
	unsigned char update = 0;
	while (count--) {
		switch (parameters->type) {
		case (MAX5825_PARAMETER_CHANNEL):
			channels = parameters->value;
			break;
		case (MAX5825_PARAMETER_VALUE):
			value = parameters->value;
			break;
		case (MAX5825_PARAMETER_UPDATE):
			update = 1;
			break;
		case (MAX5825_PARAMETER_RETURN_VALUE):
			return_val = parameters->value;
			break;
		}
		parameters++;
	}
	if (return_val != -1) {
		device_max5825_write_return(max5825, channels, return_val);
	}
	if (value != -1) {
		if (update) {
			device_max5825_write_code_load(max5825, channels, value);
		} else {
			device_max5825_write_code(max5825, channels, value);
		}
	} else {
		if (update) {
			device_max5825_load(max5825, channels);
		}
	}
}

static void device_max5825_update(void* const ads1115) {
}

static void device_max5825_send_data(void* const _max5825,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_max5825_t* max5825 = (device_max5825_t*) _max5825;

	data_package_t package = { id, max5825_not_responding, DATA_TYPE_BYTE,
			&max5825->error_command, timestamp };
	max5825->error_out->new_data(max5825->error_out->parent, &package);
}

void device_max5825_set_i2c_address(device_max5825_t* const max5825,
		const unsigned char i2c_address) {
	max5825->i2c_address = (1 << 5) | ((i2c_address & 0b1111) << 1);
}

static void device_max5825_send_command(device_max5825_t* const max5825,
		unsigned char* command) {
	if (!i2c_action_w_retry(max5825->i2c_master, max5825->i2c_address, 3, command,
			i2c_write,
			MAX5825_I2C_RETRIES)) {
		max5825->error_command = *command;
		datastream_source_generate_software_timestamp(
				(datastream_source_t*) max5825);
	}
}

static void device_max5825_set_reference(device_max5825_t* const max5825,
		max5825_reference_setting reference_setting) {
	unsigned char data[3] = { 0b00100100 | reference_setting, 0, 0 };  //TODO power mode by user?
	device_max5825_send_command(max5825, data);
}

void device_max5825_set_power_mode(device_max5825_t* const max5825,
		const unsigned char dac_mask, max5825_power_mode power_mode) {
	unsigned char data[3] = { 0b01000000, dac_mask, power_mode << 6 };
	device_max5825_send_command(max5825, data);
}

void device_max5825_set_default(device_max5825_t* const max5825,
		const unsigned char dac_mask, max5825_default_value default_value) {
	unsigned char data[3] = { 0b01100000, dac_mask, default_value << 5 };
	device_max5825_send_command(max5825, data);
}

void device_max5825_set_config(device_max5825_t* const max5825,
		const unsigned char dac_mask, max5825_wdog_config wdog,
		max5825_gate_enable sw_gate, max5825_load_enable load_enable,
		max5825_clear_enable clear_enable) {
	unsigned char data[3] = { 0b01010000, dac_mask, (wdog << 6) | (sw_gate << 5)
			| (load_enable << 4) | (clear_enable << 3) };
	device_max5825_send_command(max5825, data);
}

void device_max5825_load(device_max5825_t* const max5825,
		max5825_selection dac_sel) {
	unsigned char data[3] = { 0b10010000 | dac_sel, 0, 0 };
	device_max5825_send_command(max5825, data);
}

void device_max5825_write_return(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value) {
	unsigned char data[3] = { 0b01110000 | dac_sel, value >> 4, value << 4 };
	device_max5825_send_command(max5825, data);
}

void device_max5825_write_code(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value) {
	unsigned char data[3] = { 0b10000000 | dac_sel, value >> 4, value << 4 };
	device_max5825_send_command(max5825, data);
}

void device_max5825_write_code_load(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value) {
	unsigned char data[3] = { 0b10110000 | dac_sel, value >> 4, value << 4 };
	device_max5825_send_command(max5825, data);
}

void device_max5825_write_code_load_all(device_max5825_t* const max5825,
		max5825_selection dac_sel, unsigned int value) {
	unsigned char data[3] = { 0b10100000 | dac_sel, value >> 4, value << 4 };
	device_max5825_send_command(max5825, data);
}
