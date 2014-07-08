/**
 * @file control_protocol_ascii.c
 * @brief ASCII control protocol
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/sink/control_protocol_ascii.h>

/**
 * @brief parse function of the ascii control protocol
 *
 * @param	_protocol	pointer to the protocol struct
 * @param	new_byte	the incoming data byte
 */
static void control_protocol_ascii_parse(void* const _protocol,
		unsigned char new_byte);

void control_protocol_ascii_init(
		control_protocol_ascii_t* const control_protocol_ascii) {
	control_protocol_ascii->super.parse = control_protocol_ascii_parse;
	control_protocol_ascii->start_char = PROTOCOL_ASCII_DEFAULT_START_CHAR;
	control_protocol_ascii->separator = PROTOCOL_ASCII_DEFAULT_SEPARATOR;
	control_protocol_ascii->end_char = PROTOCOL_ASCII_DEFAULT_END_CHAR;
	control_protocol_ascii->out_select_char =
	PROTOCOL_ASCII_DEFAULT_OUT_SELECT_CHAR;

	control_protocol_ascii->state = -1;
}

void control_protocol_ascii_set_special_chars(
		control_protocol_ascii_t* const control_protocol_ascii, char start_char,
		char separator, char end_char, char out_sel_char) {
	control_protocol_ascii->start_char = start_char;
	control_protocol_ascii->separator = separator;
	control_protocol_ascii->end_char = end_char;
	control_protocol_ascii->out_select_char = out_sel_char;
}

static void control_protocol_ascii_parse(void* const _protocol,
		unsigned char new_byte) {
	control_protocol_ascii_t* protocol = (control_protocol_ascii_t*) _protocol;

	if (new_byte == protocol->start_char) {
		protocol->state = 0;
		protocol->parsed_param_count = 0;
		protocol->output = 0;
	} else {
		if (protocol->state != -1) {  //parsing a valid command?
			if ((new_byte == protocol->separator)
					|| (new_byte == protocol->end_char)) {  //end of parameter
				if (protocol->state == 1) {  //valid parameter parsed
					if (protocol->parsed_parameters[protocol->parsed_param_count].type
							!= protocol->out_select_char) {  //not output selection parameter
						protocol->parsed_parameters[protocol->parsed_param_count++].value =
								protocol->cur_value;
						if (protocol->parsed_param_count >= PROTOCOL_ASCII_MAX_PARAMETERS)
							protocol->parsed_param_count = 0;  //TODO some error handling
					} else {
						protocol->output = protocol->cur_value;
					}
					protocol->state = 0;
				} else {
					protocol->state = -1;  //error, invalidate state, wait for next start
				}
			} else {  //not a special character
				if (protocol->state == 0) {  //first char of parameter is its type
					protocol->parsed_parameters[protocol->parsed_param_count].type =
							new_byte;
					protocol->cur_value = 0;
					protocol->state = 1;
				} else {
					if ((new_byte >= '0') && (new_byte <= '9')) {
						protocol->cur_value *= 10;
						protocol->cur_value += new_byte - '0';
					} else
						protocol->state = -1;
				}
			}

			if (new_byte == protocol->end_char) {
				protocol->state = -1;  //invalidate state
				if (protocol->parsed_param_count) {  //if we have parsed something send it!
					protocol->super.send_message(protocol->super.param, protocol->output,
							protocol->parsed_param_count, protocol->parsed_parameters);
				}
			}
		}
	}
}
