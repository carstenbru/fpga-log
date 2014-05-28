/**
 * @file control_protocol.h
 * @brief abstract control protocol super-"class"
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef CONTROL_PROTOCOL_H_
#define CONTROL_PROTOCOL_H_

#include "control_port.h"

/**
 * @brief struct defining a control protocol
 *
 * This should be used as super-"class" for protocol implementations.
 */
typedef struct {
	/**
	 * @brief pointer to the parse function of one implementation
	 *
	 * @param	protocol	pointer to this protocol struct
	 * @param	new_byte	the incoming data byte
	 */
	void (*parse)(void* const protocol, unsigned char new_byte);

	/**
	 * @brief send message function which will be used to send new messages
	 *
	 * @param param				a (fixed) paramter which can be set with pointer "param", e.g. the uart sink
	 * @param output			number of the output to send the message to, 0 means all control outs
	 * @param count				amount of passed parameter structs
	 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
	 */
	void (*send_message)(void* const param, unsigned int output,
			unsigned int count, const control_parameter_t* parameters);

	void* param; /**< parameter which will be passed to send_message function */
} control_protocol_t;

/**
 * @brief stets the send_message function of the protocol analyzer
 *
 * @param control_protocol	pointer to the control_protocol
 * @param	send_message			function which will be called to send messages
 * @param	param							parameter which will be passed to send_message function
 */
void control_protocol_set_message_dest(
		control_protocol_t* const control_protocol,
		void (*send_message)(void* const param, unsigned int output,
				unsigned int count, const control_parameter_t* parameters),
		void* const param);

#endif
