/**
 * @file sdcard.c
 * @brief sdcard peripheral functions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/peripheral_funcs/sdcard.h>

#include <peripherals/uart_light.h>

#define SD_RESPONSE_WAIT 16
#define SD_INIT_WAIT 1024

/**
 * waits for the sdcard peripheral to be ready
 *
 * @param sdcard	pointer to the sdcard peripheral
 */
static sdcard_busy_wait(sdcard_regs_t* const sdcard) {
	while (sdcard->trans_sts)
		;
}

static void sdcard_reset_cs(sdcard_regs_t* const sdcard) {
	sdcard->trans_type = TRANS_TYPE_DIRECT_ACCES;
	sdcard->direct_acces_data = 0xFF;  //dummy
	sdcard->trans_ctrl = TRANS_START;
}

unsigned char sdcard_send_cmd(sdcard_regs_t* const sdcard, unsigned char cmd,
		unsigned char* data, unsigned char checksum) {
	sdcard->trans_type = TRANS_TYPE_INIT_SD;
	sdcard->direct_acces_data = 0xFF;  //dummy
	sdcard->trans_ctrl = TRANS_START;
	sdcard_busy_wait(sdcard);

	sdcard->direct_acces_data = 0x40 | cmd;  //command code
	sdcard->trans_ctrl = TRANS_START;
	sdcard_busy_wait(sdcard);

	int i;
	for (i = 0; i < 4; i++) {
		sdcard->direct_acces_data = *data++;  //data
		sdcard->trans_ctrl = TRANS_START;
		sdcard_busy_wait(sdcard);
	}

	sdcard->direct_acces_data = checksum;  //checksum
	sdcard->trans_ctrl = TRANS_START;
	sdcard_busy_wait(sdcard);

	sdcard->direct_acces_data = 0xFF;  //wait for R1 response
	for (i = 0; i < SD_RESPONSE_WAIT; i++) {
		sdcard->trans_ctrl = TRANS_START;
		sdcard_busy_wait(sdcard);
		if (sdcard->direct_acces_data != 0xFF) {
			return sdcard->direct_acces_data;
		}
	}

	return 0xFF;
}

int sdcard_init(sdcard_regs_t* const sdcard) {
	unsigned char data_empty[4] = { 0, 0, 0, 0 };
	unsigned char data_cmd41[4] = { 0b01000000, 0, 0, 0 };
	unsigned char data_cmd8[4] = { 0, 0, 0x01, 0xAA };
	unsigned char r1;
	unsigned char hcs = 0;

	r1 = sdcard_send_cmd(sdcard, 0, data_empty, 0x95);  //send CMD0
	if (r1 != 0x01) {
		return INIT_CMD0_ERROR;
	}

	sdcard_send_cmd(sdcard, 8, data_cmd8, 0x87);  //send CMD8
	int i;
	for (i = 0; i < 4; i++) {  //read R7 response
		sdcard->trans_ctrl = TRANS_START;
		sdcard_busy_wait(sdcard);
	}

	for (i = 0; i < SD_INIT_WAIT; i++) {
		sdcard_send_cmd(sdcard, 55, data_empty, 1);  //send ACMD41 (CMD55+CMD41)
		r1 = sdcard_send_cmd(sdcard, 41, data_cmd41, 1);
		if (r1 != 0x01) {  //repeat until state change
			break;
		}
	}

	if (r1 == 0x05) {  //if ACMD41 was rejected as illegal command retry with CMD1
		r1 = sdcard_send_cmd(sdcard, 1, data_cmd41, 1);
	}

	if (r1 & 0x01) {
		sdcard_reset_cs(sdcard);
		return INIT_ACMD41_ERROR;
	}

	r1 = sdcard_send_cmd(sdcard, 58, data_empty, 1);  //send CMD58 (get OCR)
	sdcard->direct_acces_data = 0xFF;
	for (i = 0; i < 4; i++) {  //read R3 response

		sdcard->trans_ctrl = TRANS_START;
		sdcard_busy_wait(sdcard);
		if (i == 0) {
			if (sdcard->direct_acces_data & 0b01000000) {  //HCS flag set
				hcs = 1;
			}
		}
	}

	sdcard_reset_cs(sdcard);

	if (r1 != 0x00) {
		return INIT_CMD58_ERROR;
	}
	return hcs ? SD_HCS_SET : SD_NO_ERROR;
}

/**
 * sets the address register in a sdcard peripheral
 *
 * @param sdcard	pointer to the sdcard peripheral
 * @param address	new address value
 */
static void sdcard_set_address(sdcard_regs_t* const sdcard,
		unsigned long int address) {
	sdcard->sd_addr_7_0 = (unsigned int) address;
	sdcard->sd_addr_15_8 = (unsigned int) (address >> 8);
	sdcard->sd_addr_23_16 = (unsigned int) (address >> 16);
	sdcard->sd_addr_31_24 = (unsigned int) (address >> 24);
}

int sdcard_block_write(sdcard_regs_t* const sdcard, unsigned long address,
		const unsigned char* block) {
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
	sdcard->rx_fifo_control = FIFO_FORCE_EMPTY;

	return SD_NO_ERROR;
}
