/**
 * @file src/i2c_funcs.c
 * @brief functions to control SpartanMC's i2c-master peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/peripheral_funcs/i2c_funcs.h>

void i2c_init(i2c_master_regs_t* i2c, int prescaler) {
  i2c->control = prescaler | I2C_EN;
}

int i2c_write(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data) {
	i2c->tx = address & 254;
	i2c->command = I2C_STA | I2C_WR;
	while (i2c->status & I2C_TIP)
		;
	if (i2c->status & I2C_RXACK)
		return 0;

	while (count--) {
		i2c->tx = *data++;

		if (count)
			i2c->command = I2C_WR;
		else
			i2c->command = I2C_WR | I2C_STO;

		while (i2c->status & I2C_TIP)
			;
		if (i2c->status & I2C_RXACK)
			return 0;
	}

	return 1;
}

int i2c_read(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data) {
	i2c->tx = address | 1;
	i2c->command = I2C_STA | I2C_WR;
	while (i2c->status & I2C_TIP)
		;
	if (i2c->status & I2C_RXACK)
		return 0;

	while (count--) {
		if (count)
			i2c->command = I2C_RD | I2C_ACK;
		else
			i2c->command = I2C_RD | I2C_STO | I2C_ACK;
		while (i2c->status & I2C_TIP)
			;
		*data++ = i2c->rx;
	}

	return 1;
}

int i2c_action_w_retry(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data,
		int (*i2c_func)(i2c_master_regs_t* i2c, int address, int count,
				unsigned char* data), int retries) {
	while (retries--) {
		if (i2c_func(i2c, address, count, data))
			return 1;
	}
	return 0;
}
