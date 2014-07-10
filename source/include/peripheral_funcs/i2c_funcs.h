/**
 * @file i2c_funcs.h
 * @brief functions to control SpartanMC's i2c-master peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef I2C_FUNCS_H_
#define I2C_FUNCS_H_

#include <peripherals/i2c_master.h>

/**
 * @brief enables a i2c master and sets clock divider
 *
 * @param i2c_master_regs_t the i2c-master to enable
 * @param prescaler					clock prescaler value
 */
void i2c_init(i2c_master_regs_t* i2c, int prescaler);

/**
 * @brief reads 16-bit of data from an i2c slave
 *
 * This function blocks until the action is finished (or ack missed).
 *
 * @param i2c_master_regs_t the i2c master
 * @param address 					the i2c slave address
 * @param count 						amount of	bytes to read
 * @param data 							pointer where the read data should be placed
 *
 * @return 1 on success, 0 if an ack failed
 */
int i2c_read(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data);

/**
 * @brief writes data to an i2c bus
 *
 * This function blocks until the action is finished (or ack missed).
 *
 * @param i2c_master_regs_t the i2c master
 * @param address 					the i2c slave address
 * @param count 						amount of	bytes to write
 * @param data							poiter to first byte which should be written
 *
 * @return 1 on success, 0 if an ack failed
 */
int i2c_write(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data);

#endif
