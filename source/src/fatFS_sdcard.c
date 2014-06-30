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

	int err_code = sdcard_init(sink->sd_card_regs);

	if (err_code == INIT_NO_ERROR) {
		sink->status = 0;
	} else {
		sink->status = STA_NOINIT;
	}

	return sink->status;
}

DSTATUS disk_status(BYTE pdrv) {
	return sink_sd_card_from_pdrv(pdrv)->status;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	int res = READ_NO_ERROR;

	int i;
	for (i = 0; i < count; i++) {
		res |= sdcard_block_read(sink->sd_card_regs, sector * SD_BLOCK_SIZE, buff);
		buff += SD_BLOCK_SIZE;
	}

	return (res == READ_NO_ERROR) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	int res = WRITE_NO_ERROR;

	int i;
	for (i = 0; i < count; i++) {
		res |= sdcard_block_write(sink->sd_card_regs, sector * SD_BLOCK_SIZE, buff);
		buff += SD_BLOCK_SIZE;
	}

	return (res == WRITE_NO_ERROR) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	//all commands not needed in our configuration

	return RES_OK;
}
