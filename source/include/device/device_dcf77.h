/**
 * @file device_dcf77.h
 * @brief DCF77 Encoder module
 *
 * @author Stefan Klir (Stefan.Klir@web.de)
 */

#ifndef DCF77_H_
#define DCF77_H_

#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>

#include <peripherals/dcf77.h>


typedef enum {
	DCF_SYNC_DISABLE = 0, /**< no synchronization */
	DCF_SYNC_TIME_AND_DATE = 1 /**< synchronize data logger time and date to DCF time and date **/
} device_dcf77_sync_mode;

typedef struct {
	datastream_source_t super; /**< super-"class": datastream_object_t*/

	const data_port_t* data_out; /**< data output (on request) destination */

	dcf77_regs_t* dcf77;

	device_dcf77_sync_mode sync_mode;

} device_dcf77_t;

void device_dcf77_init(device_dcf77_t* const dcf77,
		dcf77_regs_t* dcf77_peri, const unsigned int id, device_dcf77_sync_mode sync_mode);

void device_dcf77_set_data_out(
		device_dcf77_t* const dcf77,
		const data_port_t* const data_out);

#endif
