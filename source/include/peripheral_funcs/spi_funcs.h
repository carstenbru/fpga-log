/**
 * @file spi_funcs.h
 * @brief additional functions to control SpartanMC's spi-master peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SPI_FUNCS_H_
#define SPI_FUNCS_H_

#include <peripherals/spi_master.h>

/**
 * @brief returns the trans_empty flag of the spi peripheral and with that whether the transaction is finished
 *
 * @param spi	the spi-master
 * @return the trans_empty flag
 */
int spi_trans_finished(spi_master_regs_t* spi);

/**
 * @brief return the last read data from data_in register
 *
 * @param spi	the spi-master
 * @return	the content of the data_in register
 */
int spi_read_data_in(spi_master_regs_t* spi);

#endif
