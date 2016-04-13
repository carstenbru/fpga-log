/**
 * @file sdcard.h
 * @brief sdcard peripheral functions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SDCARD_H_
#define SDCARD_H_

#include <peripherals/sdcard.h>

/**
 * initializes a connected sdcard
 *
 * @param sdcard	pointer to the sdcard peripheral
 * @return INIT_NO_ERROR on success, otherwise error code
 */
int sdcard_init(sdcard_regs_t* const sdcard);

/**
 * writes a block (512 bytes) to the sdcard
 *
 * @param sdcard	pointer to the sdcard peripheral
 * @param address	destination address (on sdcard) of the block write
 * @param block		pointer to the data block in memory
 * @return	WRITE_NO_ERROR on success, otherwise error code
 */
//int sdcard_block_write(sdcard_regs_t* const sdcard, unsigned long address, const unsigned char* block);
int sdcard_block_write(sdcard_regs_t* const sdcard, sdcard_dma_t* const sdcard_dma, unsigned long address, const unsigned char* block);

/**
 * reads a block (512 bytes) from the sdcard
 *
 * @param sdcard	pointer to the sdcard peripheral
 * @param address	source address (on sdcard) of the block read
 * @param block		pointer to the data destination in memory
 * @return	READ_NO_ERROR on success, otherwise error code
 */
int sdcard_block_read(sdcard_regs_t* const sdcard, sdcard_dma_t* sdcard_dma, unsigned long address, unsigned char* block);

#endif
