/**
 * @file control_protocol_ascii.h
 * @brief ASCII control protocol
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef CONTROL_PROTOCOL_ASCII_H_
#define CONTROL_PROTOCOL_ASCII_H_

#include <fpga-log/sink/control_protocol.h>

#define PROTOCOL_ASCII_MAX_PARAMETERS 8

#define PROTOCOL_ASCII_DEFAULT_START_CHAR ':'
#define PROTOCOL_ASCII_DEFAULT_SEPARATOR ','
#define PROTOCOL_ASCII_DEFAULT_END_CHAR ';'
#define PROTOCOL_ASCII_DEFAULT_OUT_SELECT_CHAR 'o'

/**
 * @brief struct describing the ascii control protocol
 */
typedef struct {
	control_protocol_t super; /**< super-"class": control_protocol_t */

	unsigned char start_char; /**< command start char */
	unsigned char separator; /**< parameter separator char */
	unsigned char end_char; /**< command end char */
	unsigned char out_select_char; /**< control output select char */

	unsigned int output;
	int state;
	unsigned int cur_value;

	unsigned int parsed_param_count; /**< number of already parsed parameters in current command */
	control_parameter_t parsed_parameters[PROTOCOL_ASCII_MAX_PARAMETERS]; /**< already parsed parameters in current command */
} control_protocol_ascii_t;

/**
 * @brief init function of control_protocol_ascii
 *
 * @param control_protocol_ascii	pointer to the ascii control protocol struct
 */
void control_protocol_ascii_init(
		control_protocol_ascii_t* const control_protocol_ascii);

/**
 * @brief sets special characters of control_protocol_ascii
 *
 * This function can be used to change the start,end and separator chars of the ASCII protocol.
 *
 * @param control_protocol_ascii	pointer to the ascii control protocol struct
 * @param start_char							command start char
 * @param separator								parameter separator char
 * @param end_char								command end char
 * @param out_sel_char						control output selection parameter character
 */
void control_protocol_ascii_set_special_chars(
		control_protocol_ascii_t* const control_protocol_ascii,
		unsigned char start_char, unsigned char separator, unsigned char end_char,
		unsigned char out_sel_char);

#endif
