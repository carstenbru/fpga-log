/**
 * @file fatFS_linux_blk.c
 * @brief FatFs disk I/O layer for linux block devices
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "FatFs/ff.h"
#include "FatFs/diskio.h"
#include "sink/sink_sd_card.h"

/**
 * @brief returns current time packed into a DWORD variable
 *
 * @return current time
 */
DWORD get_fattime(void) {
	time_t curtime = time(NULL);

	struct tm* now;
	now = localtime(&curtime);

	return ((DWORD) (now->tm_year - 1980) << 25) | ((DWORD) now->tm_mon << 21)
			| ((DWORD) now->tm_mday << 16) | ((DWORD) now->tm_hour << 11)
			| ((DWORD) now->tm_min << 5) | ((DWORD) now->tm_sec >> 1);
}

DSTATUS disk_initialize(BYTE pdrv) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);

	int file = open((char*)sink->sd_card_regs, O_RDWR);  //TODO path

	if (file == -1) {
		sink->status = STA_NOINIT;
	} else {
		sink->sd_card_regs = (sdcard_regs_t*) file;
		sink->status = 0;
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

	int blk_dev = (int) sink->sd_card_regs;

	lseek(blk_dev, sector * SD_BLOCK_SIZE, SEEK_SET);
	int res = read(blk_dev, buff, count * SD_BLOCK_SIZE);

	return (res != -1) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	int blk_dev = (int) sink->sd_card_regs;

	lseek(blk_dev, sector * SD_BLOCK_SIZE, SEEK_SET);
	int res = write(blk_dev, buff, count * SD_BLOCK_SIZE);

	return (res != -1) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
	sink_sd_card_t* sink = sink_sd_card_from_pdrv(pdrv);
	if (sink->status == STA_NOINIT)
		return RES_NOTRDY;

	//all commands not needed in our configuration

	return RES_OK;
}
