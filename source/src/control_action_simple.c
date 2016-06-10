/**
 * @file control_action_simple.c
 * @brief simple control action which sends always the same commands to its control port
 *
 * This can be used for example as an action performed on a trigger or timer event.
 * Use this as super-"class" for more complex conditions.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/control_action_simple.h>

//TODO remove multi-command capabilities here and add a "control_action_multi" which holds multiple control actions
//			and calls all of them? would be more generic and reduce memory usage for simple actions
//			but more objects would be needed for more complex operations...

/**
 * @brief execute function of the simple control action
 *
 * @param _control_action_simple	the control_action_simple_t
 * @param port										pointer to the control port to which the command should be sent
 */
static void control_action_simple_execute(void* const _control_action_simple,
		const control_port_t* port) {
	control_action_simple_t* control_action_simple =
			(control_action_simple_t*) _control_action_simple;

	int i;
	for (i = 0; i < control_action_simple->configured_commands; i++) {
		port->new_control_message(port->parent,
				control_action_simple->configured_parameters[i],
				control_action_simple->command_list[i]);
	}
}

void control_action_simple_init(
		control_action_simple_t* const control_action_simple) {
	control_action_simple->super.execute = control_action_simple_execute;

	control_action_simple->configured_commands = 0;

	control_action_simple_add_command(control_action_simple);
}

void control_action_simple_add_command(
		control_action_simple_t* const control_action_simple) {
	control_action_simple->configured_parameters[control_action_simple->configured_commands++] =
			0;
}

void control_action_simple_add_paramter(
		control_action_simple_t* const control_action_simple,
		control_parameter_t* parameter) {
	unsigned int cur_command = control_action_simple->configured_commands - 1;
	control_action_simple->command_list[cur_command][control_action_simple->configured_parameters[cur_command]] =
			*parameter;
	control_action_simple->configured_parameters[cur_command]++;
}

void control_action_simple_add_paramter_exp(
		control_action_simple_t* const control_action_simple, parameter_type_t type,
		int value) {
	control_parameter_t p = { type, value };
	control_action_simple_add_paramter(control_action_simple, &p);
}
