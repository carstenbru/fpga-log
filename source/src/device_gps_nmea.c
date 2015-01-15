/**
 * @file device_gps_nmea.c
 * @brief GPS receiver device driver (NMEA protocol)
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_gps_nmea.h>
#include <fpga-log/peripheral_funcs/uart_light_funcs.h>
#include <fpga-log/long_int.h>

//TODO fix error on "course" field being reported when not present...just on real hardware
//TODO time gets negative -> overflow? perhaps only a print (formatter_simple) problem, %d is used but can be too small
//TODO implement more NMEA sentence types
//TODO options to enable/disable output of specific data fields?
const char* nmea_rmc_names[12] = NMEA_RMC_NAMES;
const char* nmea_gga_names[14] = NMEA_GGA_NAMES;
const char* nmea_vtg_names[9] = NMEA_VTG_NAMES;
const char* nmea_unknown = NMEA_UNKNOWN;

/**
 * @brief gps device send data function
 *
 * @param	_gps_nmea		pointer to the gps device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_gps_nmea_send_data(void* const _gps_nmea,
		const unsigned int id, const timestamp_t* const timestamp);

void device_gps_nmea_init(device_gps_nmea_t* const gps_nmea,
		uart_light_regs_t* const uart_light, const int id) {
	datastream_source_init(&gps_nmea->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	gps_nmea->super.send_data = device_gps_nmea_send_data;

	gps_nmea->raw_data_out = &data_port_dummy;
	gps_nmea->parsed_data_out = &data_port_dummy;

	gps_nmea->uart_light = uart_light;

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
	gps_nmea->parse_int = 0;
	gps_nmea->parse_package.data = &gps_nmea->parse_int;
	gps_nmea->parsed_digits = 0;
}

static void device_gps_nmea_set_val_type(device_gps_nmea_t* const gps_nmea) {
	if (gps_nmea->cur_sentence[0] == 'R' && gps_nmea->cur_sentence[1] == 'M'
			&& gps_nmea->cur_sentence[2] == 'C') {
		gps_nmea->parse_package.val_name =
				nmea_rmc_names[gps_nmea->parse_status - 8];
	} else if (gps_nmea->cur_sentence[0] == 'G'
			&& gps_nmea->cur_sentence[1] == 'G' && gps_nmea->cur_sentence[2] == 'A') {
		gps_nmea->parse_package.val_name =
				nmea_gga_names[gps_nmea->parse_status - 8];
	} else if (gps_nmea->cur_sentence[0] == 'V'
			&& gps_nmea->cur_sentence[1] == 'T' && gps_nmea->cur_sentence[2] == 'G') {
		gps_nmea->parse_package.val_name =
				nmea_vtg_names[gps_nmea->parse_status - 8];
	} else {
		gps_nmea->parse_package.val_name = nmea_unknown;
	}

}

static void device_gps_nmea_send_data(void* const _gps_nmea,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_gps_nmea_t* gps_nmea = (device_gps_nmea_t*) _gps_nmea;

	unsigned char byte;
	if (uart_light_receive_nb(gps_nmea->uart_light, &byte) == UART_OK) {
		data_package_t package = { id, "byte", DATA_TYPE_BYTE, &byte, timestamp };
		gps_nmea->raw_data_out->new_data(gps_nmea->raw_data_out->parent, &package);

		if (gps_nmea->parsed_data_out != &data_port_dummy) {  //parse only if data out assigned
			if (byte == '$') {  //start of a NMEA sentence
				gps_nmea->parse_status = 1;
				device_gps_nmea_reset_parse_data(gps_nmea);
			} else if ((gps_nmea->parse_status < 3)
					|| (gps_nmea->parse_status == 6)) {  //every sentence starts with "GP"
				gps_nmea->parse_status++;
			} else if (gps_nmea->parse_status < 6) {  //next three chars are the sentence type
				gps_nmea->cur_sentence[gps_nmea->parse_status - 3] = byte;
				gps_nmea->parse_status++;
			} else if (byte == ',' || byte == '*' || byte == '\r') {  //end of value markers
				gps_nmea->parse_status++;
				gps_nmea->parse_package.timestamp = timestamp;
				device_gps_nmea_set_val_type(gps_nmea);

				if (gps_nmea->parse_package.type == DATA_TYPE_SIMPLE_FLOAT) {
					unsigned char i;
					for (i = 0; i < (4 - gps_nmea->parsed_digits); i++) {
						gps_nmea->parse_int *= 10;
					}
					gps_nmea->parse_float.coefficient += cast_to_ulong(
							gps_nmea->parse_int);
				}

				if ((gps_nmea->parse_package.type != DATA_TYPE_INT)
						|| gps_nmea->parsed_digits) {
					gps_nmea->parsed_data_out->new_data(gps_nmea->parsed_data_out->parent,
							&gps_nmea->parse_package);
				}

				device_gps_nmea_reset_parse_data(gps_nmea);
			} else {
				char digit = byte - '0';
				if (digit >= 0 && digit <= 9) {  //normal digit
					gps_nmea->parse_int *= 10;
					gps_nmea->parse_int += digit;
					gps_nmea->parsed_digits++;
				} else {
					gps_nmea->parsed_digits = 0;
					if (byte == '.') {  //fixed point value -> treat as float with exponent 0
						gps_nmea->parse_float.coefficient = mul34_17(
								cast_to_ulong(gps_nmea->parse_int), 10000);
						gps_nmea->parse_int = 0;
						gps_nmea->parse_package.type = DATA_TYPE_SIMPLE_FLOAT;
						gps_nmea->parse_package.data = &gps_nmea->parse_float;
					} else if (byte != ' ') {  //single character
						gps_nmea->parse_package.type = DATA_TYPE_CHAR;
						gps_nmea->parse_byte = byte;
						gps_nmea->parse_package.data = &gps_nmea->parse_byte;
					}
				}
			}
		}
	}
}
