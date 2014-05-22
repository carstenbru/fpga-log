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
	TRANS_TYPE_DIRECT_ACCES = 0,
	TRANS_TYPE_INIT_SD = 1,
	TRANS_TYPE_RW_READ_SD_BLOCK = 2,
	TRANS_TYPE_RW_WRITE_SD_BLOCK = 3
} transaction_type;

#define TRANS_START 1

#define FIFO_FORCE_EMPTY 1

/**
 * @brief enumeration of the different sdcard initialization error codes
 */
typedef enum {
	INIT_NO_ERROR = 0,
	INIT_CMD0_ERROR = 1,
	INIT_CMD1_ERROR = 2
} init_code;

/**
 * @brief enumeration of the different sdcard read error codes
 */
typedef enum {
	READ_NO_ERROR = 0,
	READ_CMD_ERROR = 1,
	READ_TOKEN_ERROR = 2
} read_code;

/**
 * @brief enumeration of the different sdcard write error codes
 */
typedef enum {
	WRITE_NO_ERROR = 0,
	WRITE_CMD_ERROR = 1,
	WRITE_DATA_ERROR = 2,
	WRITE_BUSY_ERROR = 3
} write_code;

typedef volatile struct {
    volatile unsigned int version;
    volatile unsigned int control;
    volatile unsigned int trans_type;
    volatile unsigned int trans_ctrl;
    volatile unsigned int trans_sts;
    volatile unsigned int trans_error;
    volatile unsigned int direct_acces_data;
    volatile unsigned int sd_addr_7_0;
    volatile unsigned int sd_addr_15_8;
    volatile unsigned int sd_addr_23_16;
    volatile unsigned int sd_addr_31_24;
    volatile unsigned int spi_clk_del;

    volatile unsigned int gap1[4]; /* gap fields only used to get right address mapping of hardware registers */

    volatile unsigned int rx_fifo_data;

    volatile unsigned int gap2;

    volatile unsigned int rx_fifo_data_count_msb;
    volatile unsigned int rx_fifo_data_count_lsb;
    volatile unsigned int rx_fifo_control;

    volatile unsigned int gap3[11];

    volatile unsigned int tx_fifo_data;

    volatile unsigned int gap4[3];

    volatile unsigned int tx_fifo_control;
} sdcard_regs_t;

#endif
