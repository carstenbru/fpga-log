/**
 * @file device_tsl2591.c
 * @brief TSL2591 two channel ambient light sensor (full spectrum, infrared)
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_tsl2591.h>
#include <fpga-log/peripheral_funcs/i2c_funcs.h>
#include <fpga-log/sys_init.h>

/**
 * @brief tsl2591 send data function
 *
 * This function sends the data packages to the devices output port.
 *
 * @param	_tsl2591		pointer to the tsl2591 device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_tsl2591_send_data(void* const _tsl2591,
		const unsigned int id, const timestamp_t* const timestamp);

static void device_tsl2591_clear_intr_flag(device_tsl2591_t* const tsl2591) {
	unsigned char clear_int = 0b11100111;
	i2c_action_w_retry(tsl2591->i2c_master, TSL2591_ADDRESS, 1, &clear_int,
			i2c_write,
			TSL2591_I2C_RETRIES);  //clear interrupt flag

}

void device_tsl2591_init(device_tsl2591_t* const tsl2591,
		i2c_master_regs_t* const i2c_master, const int int_id, tsl2591_again gain,
		tsl2591_atime integration_time) {
	datastream_source_init(&tsl2591->super, int_id);  //call parents init function

	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	tsl2591->super.send_data = device_tsl2591_send_data;

	tsl2591->i2c_master = i2c_master;

	tsl2591->ch0_out = &data_port_dummy;
	tsl2591->ch1_out = &data_port_dummy;

	//from datasheet:
	//prescaler = (peripheral_clock / (5 * desired_SCL))-1
	unsigned long int prescaler = get_peri_clock();
	prescaler /= (5 * TSL2591_I2C_CLOCK);
	prescaler--;
	i2c_init(i2c_master, prescaler);

	unsigned char control[2] = { TSL2591_NORMAL_OP | TLS2591_REGISTER_CONFIG,
			0b00000000 | (gain << 4) | (integration_time) };
	i2c_action_w_retry(i2c_master, TSL2591_ADDRESS, 2, control, i2c_write,
	TSL2591_I2C_RETRIES);  //configure device gain and integration time

	unsigned char persist[2] = { TSL2591_NORMAL_OP | TLS2591_REGISTER_PERSIST,
			0b00000000 };
	i2c_action_w_retry(i2c_master, TSL2591_ADDRESS, 2, persist, i2c_write,
	TSL2591_I2C_RETRIES);  //configure device interrupt: after every measurement

	unsigned char enable[2] = { TSL2591_NORMAL_OP | TLS2591_REGISTER_ENABLE,
			0b10010011 };
	i2c_action_w_retry(i2c_master, TSL2591_ADDRESS, 2, enable, i2c_write,
	TSL2591_I2C_RETRIES);  //enable device, activate interrupt

	device_tsl2591_clear_intr_flag(tsl2591);
}

void device_tsl2591_set_ch0_out(device_tsl2591_t* const tsl2591,
		const data_port_t* const data_in) {
	tsl2591->ch0_out = data_in;
}

void device_tsl2591_set_ch1_out(device_tsl2591_t* const tsl2591,
		const data_port_t* const data_in) {
	tsl2591->ch1_out = data_in;
}

static void device_tsl2591_send_data(void* const _tsl2591,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_tsl2591_t* tsl2591 = (device_tsl2591_t*) _tsl2591;

	unsigned char set_register = TSL2591_NORMAL_OP | TLS2591_REGISTER_C0DATAL;
	i2c_action_w_retry(tsl2591->i2c_master, TSL2591_ADDRESS, 1, &set_register,
			i2c_write,
			TSL2591_I2C_RETRIES);  //set register to our data registers

	unsigned char val_buf[4];  //read the data from the device without acknowledging the last byte...otherwise it doesn't work...the device will kill the bus
	i2c_action_w_retry(tsl2591->i2c_master, TSL2591_ADDRESS, 4, val_buf,
			i2c_read_no_ack_last, TSL2591_I2C_RETRIES);

	device_tsl2591_clear_intr_flag(tsl2591);

	//send out data packages
	int ch0_val = val_buf[0] | (val_buf[1] << 8);
	int ch1_val = val_buf[2] | (val_buf[3] << 8);
	data_package_t ch0_package = { tsl2591->super.id, "TSL2591 ch0", DATA_TYPE_INT,
			&ch0_val, timestamp };
	tsl2591->ch0_out->new_data(tsl2591->ch0_out->parent, &ch0_package);
	data_package_t ch1_package = { tsl2591->super.id, "TSL2591 ch1", DATA_TYPE_INT,
			&ch1_val, timestamp };
	tsl2591->ch1_out->new_data(tsl2591->ch1_out->parent, &ch1_package);
}
