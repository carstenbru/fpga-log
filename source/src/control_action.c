/**
 * @file control_action.c
 * @brief control actions
 *
 * This can be used for example as an action performed on a trigger or timer event.
 * Use this as super-"class" for more complex conditions.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/control_action.h"

static void control_action_measure_func(void* const control_action,
		const control_port_t* port) {
	control_parameter_t p = { CONTROL_COMMAND_MEASURE, 0 };
	port->new_control_message(port->parent, 1, &p);
}

control_action_t control_action_measure = { control_action_measure_func };
