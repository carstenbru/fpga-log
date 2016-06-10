/**
 * @file control_port.c
 * @brief control_port implementations to realize control interface to device drivers
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/control_port.h>

/**
 * @brief control_port new message dummy function
 *
 * @param parent			not used
 * @param count 			not used
 * @param	parameters	not used
 */
static void control_port_new_control_message_dummy(void* const parent,
		unsigned int count, const control_parameter_t* parameters) {
}

const control_port_t control_port_dummy = { 0,
		control_port_new_control_message_dummy };

void control_parameter_init(control_parameter_t* const control_parameter,
		parameter_type_t type, int value) {
	control_parameter->type = type;
	control_parameter->value = value;
}
