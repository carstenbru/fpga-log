/**
 * @file src_PC/spi_funcs.c
 * @brief native PC functions to simulate SpartanMC's spi-master peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <spi.h>

#include <stdio.h>
#include <unistd.h>
#include <fpga-log/pc_compatibility.h>

void spi_set_div(spi_t *spi, unsigned int div) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "spi prescaler set to %d\n", div);

	write((&pipes[(int_ptr) spi])->out, &buffer, i);
}

void spi_activate(spi_t *spi, unsigned int device) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "spi slave %d selected\n", device);

	write((&pipes[(int_ptr) spi])->out, &buffer, i);
}

void spi_deactivate(spi_t *spi) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "all slaves deselected\n");

	write((&pipes[(int_ptr) spi])->out, &buffer, i);
}

void spi_enable(spi_t *spi) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "spi peripheral enabled\n");

	write((&pipes[(int_ptr) spi])->out, &buffer, i);
}

int spi_trans_finished(spi_master_regs_t* spi) {
	return 1;
}

int spi_read_data_in(spi_master_regs_t* spi) {
	unsigned char msb;
	fifo_read((int_ptr) spi, &msb);
	unsigned char lsb;
	fifo_read((int_ptr) spi, &lsb);
	return (msb << 8) | lsb;
}

void spi_write(spi_master_regs_t* spi, int data) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i = sprintf(buffer, "spi write: %d\n", data);

	write((&pipes[(int_ptr) spi])->out, &buffer, i);
}

void spi_set_cpah(spi_t* spi_master,  unsigned int cpah) {
}
