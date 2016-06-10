/**
 * @file device_gps_nmea.c
 * @brief GPS receiver device driver (NMEA protocol)
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/device/device_gps_nmea.h>
#include <fpga-log/peripheral_funcs/uart_light_funcs.h>
#include <fpga-log/long_int.h>
#include <fpga-log/sys_init.h>

//TODO fix error on "course" field being reported when not present...just on real hardware
//TODO time gets negative -> overflow? perhaps only a print (formatter_simple) problem, %d is used but can be too small
//TODO implement more NMEA sentence types
//TODO options to enable/disable output of specific data fields?
const char* nmea_rmc_names[12] = NMEA_RMC_NAMES;
const char* nmea_gga_names[14] = NMEA_GGA_NAMES;
const char* nmea_vtg_names[9] = NMEA_VTG_NAMES;
const char* nmea_unknown = NMEA_UNKNOWN;
const unsigned int time_multiplicators[8] =
		{ 36000, 3600, 600, 60, 10, 1, 10, 1 };

/**
 * @brief gps device send data function
 *
 * @param	_gps_nmea		pointer to the gps device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_gps_nmea_send_data(void* const _gps_nmea,
		const unsigned int id, const timestamp_t* const timestamp);

/**
 * @brief update function of the gps_nmea device
 *
 * @param _gps_nmea	pointer to the gps_nmea device
 */
static void device_gps_nmea_update(void* const _gps_nmea);

void device_gps_nmea_init(device_gps_nmea_t* const gps_nmea,
		uart_light_regs_t* const uart_light, device_gps_sync_mode sync_logger_time,
		const int id) {
	datastream_source_init(&gps_nmea->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	gps_nmea->super.send_data = device_gps_nmea_send_data;
	gps_nmea->super.super.update = device_gps_nmea_update;

	gps_nmea->raw_data_out = &data_port_dummy;
	gps_nmea->parsed_data_out = &data_port_dummy;

	gps_nmea->sync_logger_time = sync_logger_time;
	gps_nmea->uart_light = uart_light;

	gps_nmea->timestamp_miss_counter = 0;
	gps_nmea->timestamp_miss_assumption =
	DEVICE_GPS_NMEA_DEFAULT_TIMESTAMP_MISS_ASSUMPTION;
	gps_nmea->max_bytes_per_call = DEVICE_GPS_NMEA_DEFAULT_MAY_BYTES_PER_CALL;

	uart_light_enable_rxint(uart_light);

	gps_nmea->parse_status = 0;
	gps_nmea->parse_package.source_id = id;
}

void device_gps_nmea_set_raw_data_out(device_gps_nmea_t* const gps_nmea,
		const data_port_t* const data_in) {
	gps_nmea->raw_data_out = data_in;
}

void device_gps_nmea_set_parsed_data_out(device_gps_nmea_t* const gps_nmea,
		const data_port_t* const data_in) {
	gps_nmea->parsed_data_out = data_in;
}

static void device_gps_nmea_reset_parse_data(device_gps_nmea_t* const gps_nmea) {
	gps_nmea->parse_package.type = DATA_TYPE_INT;  //reset everything
	gps_nmea->parse_uint = 0;
	gps_nmea->parse_package.data = &gps_nmea->parse_uint;
	gps_nmea->parsed_digits = 0;
}

static void device_gps_nmea_set_val_type(device_gps_nmea_t* const gps_nmea) {
	if ((gps_nmea->cur_sentence[0] == 'R') & (gps_nmea->cur_sentence[1] == 'M')
			& (gps_nmea->cur_sentence[2] == 'C')) {
		gps_nmea->parse_package.val_name =
				nmea_rmc_names[gps_nmea->parse_status - 8];
	} else if ((gps_nmea->cur_sentence[0] == 'G')
			& (gps_nmea->cur_sentence[1] == 'G')
			& (gps_nmea->cur_sentence[2] == 'A')) {
		gps_nmea->parse_package.val_name =
				nmea_gga_names[gps_nmea->parse_status - 8];
	} else if ((gps_nmea->cur_sentence[0] == 'V')
			& (gps_nmea->cur_sentence[1] == 'T')
			& (gps_nmea->cur_sentence[2] == 'G')) {
		gps_nmea->parse_package.val_name =
				nmea_vtg_names[gps_nmea->parse_status - 8];
	} else {
		gps_nmea->parse_package.val_name = nmea_unknown;
	}

}

static void device_gps_nmea_send_data(void* const _gps_nmea,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_gps_nmea_t* gps_nmea = (device_gps_nmea_t*) _gps_nmea;

	uart_light_disable_rxint(gps_nmea->uart_light);

	gps_nmea->timestamp_miss_counter = 0;

	unsigned char byte;
	unsigned int processed_bytes = 0;
	while (uart_light_receive_nb(gps_nmea->uart_light, &byte) == UART_OK) {
		if (byte > 127) {  //filter out non ASCII characters (should not be there, but who knows..)
			gps_nmea->parse_status = 0;
			continue;
		}

		if (byte == '$') {  //found start of NMEA sentence
			gps_nmea->parse_status = 1;
		}

		if (gps_nmea->parse_status != 0) {
			data_package_t package = { id, "byte", DATA_TYPE_BYTE, &byte, timestamp };
			gps_nmea->raw_data_out->new_data(gps_nmea->raw_data_out->parent,
					&package);
		}
		if ((gps_nmea->parsed_data_out != &data_port_dummy)
				|| (gps_nmea->sync_logger_time == GPS_SYNC_TIME)) {  //parse only if data out assigned or sync activated
			if (byte == '$') {  //start of a NMEA sentence
				device_gps_nmea_reset_parse_data(gps_nmea);
			} else if (gps_nmea->parse_status != 0) {  //valid char
				if ((gps_nmea->parse_status < 3) || (gps_nmea->parse_status == 6)) {  //every sentence starts with "GP"
					gps_nmea->parse_status++;
				} else if (gps_nmea->parse_status < 6) {  //next three chars are the sentence type
					gps_nmea->cur_sentence[gps_nmea->parse_status - 3] = byte;
					gps_nmea->parse_status++;
				} else if (byte == ',' || byte == '*' || byte == '\r') {  //end of value markers
					gps_nmea->parse_status++;

					if (gps_nmea->parsed_data_out != &data_port_dummy) {
						gps_nmea->parse_package.timestamp = timestamp;
						device_gps_nmea_set_val_type(gps_nmea);

						if (gps_nmea->parse_package.type == DATA_TYPE_SIMPLE_FLOAT) {
							unsigned char i;
							for (i = 0; i < (4 - gps_nmea->parsed_digits); i++) {
								gps_nmea->parse_uint *= 10;
							}
							gps_nmea->parse_float.coefficient += cast_to_ulong(
									gps_nmea->parse_uint);
						}

						if ((gps_nmea->parse_package.type != DATA_TYPE_INT)
								|| gps_nmea->parsed_digits) {
							if (gps_nmea->parse_package.val_name != nmea_unknown)
								gps_nmea->parsed_data_out->new_data(
										gps_nmea->parsed_data_out->parent,
										&gps_nmea->parse_package);
						}
						device_gps_nmea_reset_parse_data(gps_nmea);
					}

					if (byte == '*') {
						gps_nmea->parse_status = 1;
					}
				} else {
					char digit = byte - '0';
					if (gps_nmea->parsed_data_out != &data_port_dummy) {
						if (digit >= 0 && digit <= 9) {  //normal digit
							if (((gps_nmea->parsed_digits < 4)
									&& (gps_nmea->parse_package.type == DATA_TYPE_SIMPLE_FLOAT))  //Check if there are only 4 decimal places or truncate them to 4
							|| (gps_nmea->parse_package.type == DATA_TYPE_INT)) {
								gps_nmea->parse_uint *= 10;
								gps_nmea->parse_uint += digit;
								gps_nmea->parsed_digits++;
							}
						} else {
							gps_nmea->parsed_digits = 0;
							if (byte == '.') {  //fixed point value -> treat as float with exponent 0
								gps_nmea->parse_float.coefficient = mul34_17(
										cast_to_ulong(gps_nmea->parse_uint), 10000);
								gps_nmea->parse_uint = 0;
								gps_nmea->parse_package.type = DATA_TYPE_SIMPLE_FLOAT;
								gps_nmea->parse_package.data = &gps_nmea->parse_float;
							} else if (byte != ' ') {  //single character
								gps_nmea->parse_package.type = DATA_TYPE_CHAR;
								gps_nmea->parse_byte = byte;
								gps_nmea->parse_package.data = &gps_nmea->parse_byte;
							}
						}
					}
					if (gps_nmea->sync_logger_time == GPS_SYNC_TIME) {
						if (gps_nmea->parse_status == 7) {  //first data field in a sentence
							if ((gps_nmea->cur_sentence[0] == 'G')
									& (gps_nmea->cur_sentence[1] == 'G')
									& (gps_nmea->cur_sentence[2] == 'A')) {
								if (digit >= 0 && digit <= 9) {
									if (gps_nmea->time_parse_position == 0) {
										gps_nmea->time_parsed = 0;
									}
									gps_nmea->time_parsed += digit
											* time_multiplicators[gps_nmea->time_parse_position];
									if (gps_nmea->time_parse_position == 5) {  //seconds finished
										gps_nmea->time_parsed_sec = gps_nmea->time_parsed;
										gps_nmea->time_parsed = 0;
									}
									if (gps_nmea->time_parse_position == 7) {
										timestamp_gen_regs_t* timestamp_gen = get_timestamp_gen();
										timestamp_gen->timestamp.lpt_union.lpt =
												gps_nmea->time_parsed_sec;
										unsigned long int lpt = mul34_17(get_peri_clock(),
												gps_nmea->time_parsed);
										lpt /= 10;
										timestamp_gen->timestamp.hpt_union.hpt = lpt;
									}
								}
								gps_nmea->time_parse_position++;
							}
						} else {
							gps_nmea->time_parse_position = 0;
						}
					}
				}
			}
		}
		processed_bytes++;
		if (processed_bytes >= gps_nmea->max_bytes_per_call) {
			//force generation of timestamp in next update function call
			gps_nmea->timestamp_miss_counter = gps_nmea->timestamp_miss_assumption;
			break;
		}

	}
	uart_light_enable_rxint(gps_nmea->uart_light);
}

void device_gps_nmea_set_timestamp_miss_assumption(
		device_gps_nmea_t* const gps_nmea, unsigned int timestamp_miss_assumption) {
	gps_nmea->timestamp_miss_assumption = timestamp_miss_assumption;
}

void device_gps_nmea_set_max_bytes_per_call(device_gps_nmea_t* const gps_nmea,
		unsigned int max_bytes_per_call) {
	gps_nmea->max_bytes_per_call = max_bytes_per_call;
}

static void device_gps_nmea_update(void* const _gps_nmea) {
	device_gps_nmea_t* gps_nmea = (device_gps_nmea_t*) _gps_nmea;

	if (uart_light_rx_data_available(gps_nmea->uart_light)) {
		gps_nmea->timestamp_miss_counter++;
		if (gps_nmea->timestamp_miss_counter
				>= gps_nmea->timestamp_miss_assumption) {
			gps_nmea->timestamp_miss_counter = 0;
			_datastream_source_generate_software_timestamp(
					(datastream_source_t*) gps_nmea);
		}
	} else {
		gps_nmea->timestamp_miss_counter = 0;
	}
}
