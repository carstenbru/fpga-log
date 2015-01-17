/**
 * @file sdcard.c
 * @brief sdcard peripheral functions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/peripheral_funcs/sdcard.h>

#include <peripherals/uart_light.h>

/**
 * waits for the sdcard peripheral to be ready
 *
 * @param sdcard	pointer to the sdcard peripheral
 */
static sdcard_busy_wait(sdcard_regs_t* const sdcard) {
	while (sdcard->trans_sts)
		;
}

int sdcard_init(sdcard_regs_t* const sdcard) {
	sdcard->trans_type = TRANS_TYPE_INIT_SD;
	sdcard->trans_ctrl = TRANS_START;

	sdcard_busy_wait(sdcard);

	return sdcard->trans_error;
}

/**
 * sets the address register in a sdcard peripheral
 *
 * @param sdcard	pointer to the sdcard peripheral
 * @param address	new address value
 */
static void sdcard_set_address(sdcard_regs_t* const sdcard,
		unsigned long int address) {
	sdcard->sd_addr_7_0 = address;
	sdcard->sd_addr_15_8 = address >> 8;
	sdcard->sd_addr_23_16 = address >> 16;
	sdcard->sd_addr_31_24 = address >> 24;
}

int sdcard_block_write(sdcard_regs_t* const sdcard,
		unsigned long address, const unsigned char* block) {
	int i;
	for (i = 0; i < SD_BLOCK_SIZE; i++) {  //write data to peripheral fifo
		sdcard->tx_fifo_data = *block++;
	}
	sdcard_set_address(sdcard, address);
	sdcard->trans_type = TRANS_TYPE_RW_WRITE_SD_BLOCK;
	sdcard->trans_ctrl = TRANS_START;

	sdcard_busy_wait(sdcard);
	return sdcard->trans_error;
}

int sdcard_block_read(sdcard_regs_t* const sdcard, unsigned long address,
		unsigned char* block) {
	sdcard_set_address(sdcard, address);
	sdcard->trans_type = TRANS_TYPE_RW_READ_SD_BLOCK;
	sdcard->trans_ctrl = TRANS_START;

	sdcard_busy_wait(sdcard);
	if (sdcard->trans_error != SD_NO_ERROR)
		return (sdcard->trans_error);

	/*
	 * dummy read:
	 * Since sdcard peripheral was originally designed for a wishbone bus which supports delays by the peripheral
	 * (and SpartanMCs bus does not), the fifo has a delay of one clock cycle.
	 * So the read data is always the last value and not the real one.
	 */
	*block = sdcard->rx_fifo_data;
	int i;
	for (i = 0; i < SD_BLOCK_SIZE; i++) {  //read data from peripheral fifo
		*block++ = sdcard->rx_fifo_data;
	}
	sdcard->rx_fifo_control = FIFO_FORCE_EMPTY;  //clear last pending read in peripheral, refers to the dummy read

	return SD_NO_ERROR;
}
