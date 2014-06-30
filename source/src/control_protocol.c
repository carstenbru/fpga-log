/**
 * @file control_protocol.c
 * @brief abstract control protocol super-"class"
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/sink/control_protocol.h>

void control_protocol_set_message_dest(
		control_protocol_t* const control_protocol,
		void (*send_message)(void* const param, unsigned int output,
				unsigned int count, const control_parameter_t* parameters),
		void* const param) {
	control_protocol->send_message = send_message;
	control_protocol->param = param;
}
