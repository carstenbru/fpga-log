/**
 * @file sdcard.h
 * @brief sd card peripheral registers
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef __SDCARD_H
#define __SDCARD_H

#define SD_BLOCK_SIZE 512

/**
 * @brief enumeration of the different transaction types
 */
typedef enum {
	TRANS_TYPE_DIRECT_ACCES = 0 << 1,
	TRANS_TYPE_INIT_SD = 1 << 1,
	TRANS_TYPE_RW_READ_SD_BLOCK = 2 << 1,
	TRANS_TYPE_RW_WRITE_SD_BLOCK = 3 << 1
} transaction_type;

#define TRANS_START 1
#define TRANS_STATUS (1 << 3)

#define FIFO_WR_FORCE_EMPTY (1 << 3)
#define FIFO_RD_FORCE_EMPTY (1 << 4)

/**
 * @brief enumeration of the different sdcard error codes
 */
typedef enum {
	SD_NO_ERROR = 0,
	INIT_CMD0_ERROR = 1,
	INIT_ACMD41_ERROR = 2,
	INIT_CMD58_ERROR = 3,
	READ_CMD_ERROR = 1 << 2,
	READ_TOKEN_ERROR = 2 << 2,
	WRITE_CMD_ERROR = 1 << 4,
	WRITE_DATA_ERROR = 2 << 3,
	WRITE_BUSY_ERROR = 3 << 4
} sd_trans_error_codes;

#define SD_HCS_SET (1 << 3)

typedef struct {
	volatile unsigned int version;
	volatile unsigned int control_reg;
	volatile unsigned int trans_ctrl;
	volatile unsigned int trans_error;
	volatile unsigned int direct_acces_data;
	volatile unsigned int sd_addr_17_0;
	volatile unsigned int sd_addr_31_18;
	volatile unsigned int spi_clk_del;
} sdcard_regs_t;

typedef struct {
	volatile unsigned int bytes[512];
} sdcard_dma_t;

#endif
