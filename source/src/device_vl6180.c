/**
 * @file device_vl6180.c
 * @brief VL6180 proximity and ambient light sensor
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_vl6180.h>
#include <fpga-log/peripheral_funcs/i2c_funcs.h>
#include <fpga-log/sys_init.h>

/**
 * @brief vl6180 send data function
 *
 * This function sends the data packages to the devices output port.
 *
 * @param	_vl6180		pointer to the vl6180 device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_vl6180_send_data(void* const _vl6180, const unsigned int id,
		const timestamp_t* const timestamp);

/**
 * @brief control message function of vl6180 device
 *
 * @param	_vl6180			the vl6180 device
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void device_vl6180_control_message(void* const _vl6180,
		unsigned int count, const control_parameter_t* parameters);

void device_vl6180_init(device_vl6180_t* const vl6180,
		i2c_master_regs_t* const i2c_master, const int int_id) {
	datastream_source_init(&vl6180->super, int_id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	vl6180->super.send_data = device_vl6180_send_data;

	vl6180->i2c_master = i2c_master;

	vl6180->control_in = control_port_dummy;
	vl6180->control_in.parent = (void*) vl6180;
	vl6180->control_in.new_control_message = device_vl6180_control_message;

	vl6180->range_out = &data_port_dummy;
	vl6180->als_out = &data_port_dummy;
	vl6180->error_out = &data_port_dummy;

	//from datasheet:
	//prescaler = (peripheral_clock / (5 * desired_SCL))-1
	unsigned long int prescaler = get_peri_clock();
	prescaler /= (5 * VL6180_I2C_CLOCK);
	prescaler--;
	i2c_init(i2c_master, prescaler);

	unsigned char gpio1[3] = { 0, VL6180_REGISTER_SYSTEM__MODE_GPIO1, 0b00110000 };
	i2c_action_w_retry(i2c_master, VL6180_ADDRESS, 3, gpio1, i2c_write,
	VL6180_I2C_RETRIES);  //activate GPIO1 interrupt function

	unsigned char int_cfg[3] = { 0, VL6180_REGISTER_SYSTEM__INTERRUPT_CONFIG_GPIO,
			0b00100100 };
	i2c_action_w_retry(i2c_master, VL6180_ADDRESS, 3, int_cfg, i2c_write,
	VL6180_I2C_RETRIES);  //configure interrupt (both "new sample ready")

	unsigned char itg_period_cfg[4] = { 0,
			VL6180_REGISTER_SYSALS__INTEGRATION_PERIOD, 0, 99 };
	i2c_action_w_retry(i2c_master, VL6180_ADDRESS, 4, itg_period_cfg, i2c_write,
	VL6180_I2C_RETRIES);  //datasheet recommends setting integration period to 100ms (=99)
}

control_port_t* device_vl6180_get_control_in(device_vl6180_t* const vl6180) {
	return &vl6180->control_in;
}

void device_vl6180_set_range_out(device_vl6180_t* const vl6180,
		const data_port_t* const data_in) {
	vl6180->range_out = data_in;
}

void device_vl6180_set_als_out(device_vl6180_t* const vl6180,
		const data_port_t* const data_in) {
	vl6180->als_out = data_in;
}

void device_vl6180_set_error_out(device_vl6180_t* const vl6180,
		const data_port_t* const data_in) {
	vl6180->error_out = data_in;
}

static void device_vl6180_control_message(void* const _vl6180,
		unsigned int count, const control_parameter_t* parameters) {
	device_vl6180_t* vl6180 = (device_vl6180_t*) _vl6180;

	while (count--) {
		if (parameters->type == VL6180_PARAMETER_START_RANGE) {
			unsigned char range_start[3] =
					{ 0, VL6180_REGISTER_SYSRANGE__START, 0x01 };
			i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, range_start,
					i2c_write,
					VL6180_I2C_RETRIES);
		} else if (parameters->type == VL6180_PARAMETER_START_ALS) {
			unsigned char als_start[3] = { 0, VL6180_REGISTER_SYSALS__START, 0x01 };
			i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, als_start,
					i2c_write,
					VL6180_I2C_RETRIES);
		}
		parameters++;
	}
}

static unsigned char device_vl6180_check_state(device_vl6180_t* vl6180,
		const timestamp_t* const timestamp) {
	unsigned char reg_set[2] = { 0, VL6180_REGISTER_RESULT__RANGE_STATUS };
	if (i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 2, reg_set,
			i2c_write,
			VL6180_I2C_RETRIES)) {
		unsigned char status[3];
		if (i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, status,
				i2c_read,
				VL6180_I2C_RETRIES)) {
			unsigned char err = status[0] >> 4;
			if (err) {
				data_package_t package = { vl6180->super.id, "range error",
						DATA_TYPE_BYTE, &err, timestamp };
				vl6180->error_out->new_data(vl6180->error_out->parent, &package);
			}
			err = status[1] >> 4;
			if (err) {
				data_package_t package = { vl6180->super.id, "ALS error",
						DATA_TYPE_BYTE, &err, timestamp };
				vl6180->error_out->new_data(vl6180->error_out->parent, &package);
			}
			err = status[2] >> 6;
			if (err) {
				data_package_t package = { vl6180->super.id, "system error",
						DATA_TYPE_BYTE, &err, timestamp };
				vl6180->error_out->new_data(vl6180->error_out->parent, &package);
			}
			return status[2];
		}
	}
	return 0;
}

static void device_vl6180_send_data(void* const _vl6180, const unsigned int id,
		const timestamp_t* const timestamp) {
	device_vl6180_t* vl6180 = (device_vl6180_t*) _vl6180;

	unsigned char state = device_vl6180_check_state(vl6180, timestamp);

	if (state & 0b00111000) {  //ALS value
		unsigned char als_set[2] = { 0, VL6180_REGISTER_RESULT__ALS_VAL };
		if (i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 2, als_set,
				i2c_write,
				VL6180_I2C_RETRIES)) {
			unsigned char als[2];
			if (i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 2, als,
					i2c_read,
					VL6180_I2C_RETRIES)) {
				unsigned int val = als[0] << 8 | als[1];
				data_package_t package = { id, "ALS value", DATA_TYPE_INT, &val,
						timestamp };
				vl6180->als_out->new_data(vl6180->als_out->parent, &package);
			}
		}
	}

	if (state & 0b00000111) {  //range value
		unsigned char range_set[2] = { 0, VL6180_REGISTER_RESULT__RANGE_VAL };
		if (i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 2, range_set,
				i2c_write,
				VL6180_I2C_RETRIES)) {
			unsigned char range;
			if (i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 1, &range,
					i2c_read,
					VL6180_I2C_RETRIES)) {
				data_package_t package = { id, "range value", DATA_TYPE_BYTE, &range,
						timestamp };
				vl6180->range_out->new_data(vl6180->range_out->parent, &package);
			}
		}

		//clear interrupt flag bits
		unsigned char intr_clear[3] = { 0, VL6180_REGISTER_SYSTEM__INTERRUPT_CLEAR,
				0b00000111 };
		i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, intr_clear,
				i2c_write,
				VL6180_I2C_RETRIES);
	}
}

void device_vl6180_start_range_continous(device_vl6180_t* const vl6180,
		unsigned int period_ms) {
	unsigned char range_period[3] = { 0,
			VL6180_REGISTER_SYSRANGE__INTERMEASUREMENT_PERIOD, (period_ms / 10) - 1 };
	i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, range_period,
			i2c_write,
			VL6180_I2C_RETRIES);
	unsigned char range_cont[3] = { 0, VL6180_REGISTER_SYSRANGE__START, 0x03 };
	i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, range_cont,
			i2c_write,
			VL6180_I2C_RETRIES);
}

void device_vl6180_start_als_continous(device_vl6180_t* const vl6180,
		unsigned int period_ms) {
	unsigned char als_period[3] = { 0,
			VL6180_REGISTER_SYSALS__INTERMEASUREMENT_PERIOD, (period_ms / 10) - 1 };
	i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, als_period,
			i2c_write,
			VL6180_I2C_RETRIES);
	unsigned char als_cont[3] = { 0, VL6180_REGISTER_SYSALS__START, 0x03 };
	i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, als_cont, i2c_write,
	VL6180_I2C_RETRIES);
}

void device_vl6180_start_both_continous(device_vl6180_t* const vl6180,
		unsigned int period_ms) {
	unsigned char interleaved[3] = { 0x02, 0xA3, 0x01 };  //enable interleaved mode
	i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, interleaved,
			i2c_write,
			VL6180_I2C_RETRIES);
	unsigned char als_period[3] = { 0,
			VL6180_REGISTER_SYSALS__INTERMEASUREMENT_PERIOD, (period_ms / 10) - 1 };
	i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, als_period,
			i2c_write,
			VL6180_I2C_RETRIES);
	unsigned char als_cont[3] = { 0, VL6180_REGISTER_SYSALS__START, 0x03 };
	i2c_action_w_retry(vl6180->i2c_master, VL6180_ADDRESS, 3, als_cont, i2c_write,
	VL6180_I2C_RETRIES);
}
