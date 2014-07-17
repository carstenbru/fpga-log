/**
 * @file src_PC/i2c_funcs.c
 * @brief native PC functions to simulate SpartanMC's i2c-master peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/peripheral_funcs/i2c_funcs.h>
#include <stdio.h>
#include <unistd.h>
#include <fpga-log/pc_compatibility.h>

void i2c_init(i2c_master_regs_t* i2c, int prescaler) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "i2c enabled with prescaler set to %d\n", prescaler);

	write((&pipes[(int_ptr) i2c])->out, &buffer, i);
}

int i2c_write(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "i2c write to address %d\n", address);
	write((&pipes[(int_ptr) i2c])->out, &buffer, i);
	while (count--) {
		i = sprintf(buffer, "write byte %d\n", *data++);
		write((&pipes[(int_ptr) i2c])->out, &buffer, i);
	}

	return 1;
}

int i2c_read(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "i2c read from address %d\n", address);
	write((&pipes[(int_ptr) i2c])->out, &buffer, i);

	while (count--) {
		fifo_read((int_ptr) i2c, data++);
	}

	return 1;
}

int i2c_action_w_retry(i2c_master_regs_t* i2c, int address, int count,
		unsigned char* data,
		int (*i2c_func)(i2c_master_regs_t* i2c, int address, int count,
				unsigned char* data), int retries) {
	return i2c_func(i2c, address, count, data);
}
