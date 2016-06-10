/**
 * @file src/spi_funcs.c
 * @brief additional functions to control SpartanMC's spi-master peripheral
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <spi.h>

int spi_trans_finished(spi_master_regs_t* spi) {
	return spi->spi_control & SPI_MASTER_CTRL_TRANS_EMPTY;
}

int spi_read_data_in(spi_master_regs_t* spi) {
	return spi->spi_data_in;
}
