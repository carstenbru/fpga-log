/**
 * @file device_dcf77.c
 * @brief DCF77 Encoder with data and time output
 * 		  synchronize the timestamp with the time in sec within a year
 *
 * @author Stefan Klir (s.klir@gmx.de)
 */


#include <fpga-log/device/device_dcf77.h>
#include <fpga-log/long_int.h>

//Time in sec since the begin of the current year (every year is assumed to have 28 days in Feb)
const unsigned long int month_seconds[13] =
		{ 0, 0, 2678400, 5097600, 7776000, 10368000, 13046400, 15638400, 18316800,
				20995200, 23587200, 26265600, 28857600};
const unsigned long int day_seconds = 86400;
const unsigned long int year_seconds = 31536000;

static void device_dcf77_send_data(
		void* const _dcf77, const unsigned int id,
		const timestamp_t* const timestamp);

void device_dcf77_init(device_dcf77_t* const dcf77,
		dcf77_regs_t* dcf77_peri, const unsigned int id, device_dcf77_sync_mode sync_mode, timestamp_counter_regs_t* timestamp_counter) {
	datastream_source_init(&dcf77->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	dcf77->super.send_data = device_dcf77_send_data;

	dcf77->dcf77 = dcf77_peri;

	dcf77->data_out = &data_port_dummy;

	dcf77->sync_mode = sync_mode;
	dcf77->dcf77->control = (1<<DCF77_ENABLE_INTERRUPT);
	dcf77->timestamp_counter = timestamp_counter;

}

void device_dcf77_set_data_out(
		device_dcf77_t* const dcf77,
		const data_port_t* const data_port_out) {
	dcf77->data_out = data_port_out;
}

static void device_dcf77_send_data(
		void* const _dcf77, const unsigned int id,
		const timestamp_t* const timestamp) {

	device_dcf77_t* dcf77 = (device_dcf77_t*) _dcf77;

	unsigned int minute = (dcf77->dcf77->minute & 0xff) + (((dcf77->dcf77->minute >> 8) & 0xff)*10);
	unsigned int hour = (dcf77->dcf77->hour & 0xff) + (((dcf77->dcf77->hour >> 8) & 0xff)*10);
	unsigned int day = (dcf77->dcf77->day & 0xff) + (((dcf77->dcf77->day >> 8) & 0xff)*10);
	unsigned int month = (dcf77->dcf77->month & 0xff) + (((dcf77->dcf77->month >> 8) & 0xff)*10);
	unsigned int year = (dcf77->dcf77->year & 0xff) + (((dcf77->dcf77->year >> 8) & 0xff)*10);
	unsigned long int time_in_sec = minute*60 + hour*3600;

	unsigned long int day_sec = mul34_17(day_seconds, day);
	unsigned long int year_sec = mul34_17(year_seconds, year);
	unsigned long int sum_1 = addu36(time_in_sec, month_seconds[month]);
	unsigned long int time_date_in_sec = addu36(sum_1, day_sec);

	//unsigned long int temp = addu36(72120L, 13046400L);

	data_package_t package = { dcf77->super.id, "Day",
			DATA_TYPE_INT, (void*) &day , timestamp };
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	package.val_name = "Month";
	package.data = (void*) &month;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	package.val_name = "Year";
	package.data = (void*) &year;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	package.val_name = "Hour";
	package.data = (void*) &hour;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	package.val_name = "Minute";
	package.data = (void*) &minute;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	if (dcf77->sync_mode == DCF_SYNC_TIME_AND_DATE) {
		//Time in sec since the begin of the current year (every year is assumed to have 28 days in Feb)
		dcf77->timestamp_counter->timestamp.lpt_union.lpt = time_date_in_sec;
		dcf77->timestamp_counter->timestamp.hpt_union.hpt = 0L;
	}
}

