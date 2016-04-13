/**
 * @file fatFS_sdcard.c
 * @brief FatFs disk I/O layer for sdcard peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/FatFs/ff.h>
#include <fpga-log/FatFs/diskio.h>
#include <fpga-log/sink/sink_sd_card.h>

#include <fpga-log/peripheral_funcs/sdcard.h>

/**
 * @brief returns current time packed into a DWORD variable
 *
 * @return current time
 */
DWORD get_fattime(void) {
	return 42;  //TODO
}

DSTATUS disk_initialize(BYTE pdrv) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);

	int ret_code = sdcard_init(sink->sd_card_regs);
	int err_code = ret_code & ~SD_HCS_SET;  //clear HCS flag in return value for error code

	if (ret_code & SD_HCS_SET) {
		sink->sdhc_card = 1;
	}

	if (err_code == SD_NO_ERROR) {
		sink->status = 0;
	} else {
		sink->status = STA_NOINIT;
	}

	sink->sd_error_code |= err_code;

	//sink->sdhc_card = 1; //TODO
	return sink->status;
}

DSTATUS disk_status(BYTE pdrv) {
	return sink_sd_card_from_pdrv(pdrv)->status;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	int res = SD_NO_ERROR;

	int i;
	for (i = 0; i < count; i++) {
		DWORD sec_address = sink->sdhc_card ? sector : sector * SD_BLOCK_SIZE;
		res |= sdcard_block_read(sink->sd_card_regs, sink->sd_card_dma, sec_address, buff);
		buff += SD_BLOCK_SIZE;
		sector++;
	}

	sink->sd_error_code |= res;

	return (res == SD_NO_ERROR) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	int res = SD_NO_ERROR;

	int i;
	for (i = 0; i < count; i++) {
		DWORD sec_address = sink->sdhc_card ? sector : sector * SD_BLOCK_SIZE;
		res |= sdcard_block_write(sink->sd_card_regs, sink->sd_card_dma,
				sec_address, buff);
		buff += SD_BLOCK_SIZE;
		sector++;
	}

	sink->sd_error_code |= res;

	return (res == SD_NO_ERROR) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	//all commands not needed in our configuration

	return RES_OK;
}
