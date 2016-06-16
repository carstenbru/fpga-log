#include <fpga-log/device/device_dcf77.h>
#include <fpga-log/long_int.h>

const unsigned long month_seconds[13] =
		{ 0, 0, 2678400, 5097600, 7776000, 10368000, 13046400, 15638400, 18316800,
				20995200, 23587200, 26265600, 28857600};
const unsigned long day_seconds = 86400;
const unsigned long year_seconds = 31536000;

static void device_dcf77_send_data(
		void* const _dcf77, const unsigned int id,
		const timestamp_t* const timestamp);


void device_dcf77_init(device_dcf77_t* const dcf77,
		dcf77_regs_t* dcf77_peri, const unsigned int id, device_dcf77_sync_mode sync_mode) {
	datastream_source_init(&dcf77->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	dcf77->super.send_data = device_dcf77_send_data;

	dcf77->dcf77 = dcf77_peri;

	dcf77->data_out = &data_port_dummy;

	dcf77->sync_mode = sync_mode;
	dcf77->dcf77->control = (1<<DCF77_ENABLE_INTERRUPT);
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
	unsigned int time_in_sec = minute*60 + hour*3600;
	//unsigned long int time_date_in_sec = day_seconds*(long)day + month_seconds[month] + (long)year*year_seconds + (long)time_in_sec;

	data_package_t package = { dcf77->super.id, "Day",
			DATA_TYPE_INT, (void*) &day , timestamp };
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	package.val_name = "Hour";
	package.data = (void*) &hour;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	package.val_name = "Minute";
	package.data = (void*) &minute;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	unsigned long time_date_in_sec = day_seconds*cast_to_ulong(day);
	package.val_name = "Sec_day";
	package.data = (void*) &time_date_in_sec;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	time_date_in_sec = month_seconds[month];
	package.val_name = "Sec_month";
	package.data = (void*) &time_date_in_sec;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	time_date_in_sec = cast_to_ulong(year*year_seconds);
	package.val_name = "Sec_year";
	package.data = (void*) &time_date_in_sec;
	dcf77->data_out->new_data(dcf77->data_out->parent, &package);

	time_date_in_sec = day_seconds*cast_to_ulong(day) + month_seconds[month] + cast_to_ulong(year)*year_seconds + cast_to_ulong(time_in_sec);
	if (dcf77->sync_mode == DCF_SYNC_TIME_AND_DATE) {
		timestamp_gen_regs_t* timestamp_gen = get_timestamp_gen();
		timestamp_gen->timestamp.lpt_union.lpt = time_date_in_sec;
		timestamp_gen->timestamp.hpt_union.hpt = 0L;
	}
}

