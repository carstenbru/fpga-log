/**
 * @file control_action_function.c
 * @brief control action which generates the next value of a parameter by a function
 *
 * This can be used for example as an action performed on a trigger or timer event.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/control_action_function.h>

/**
 * @brief execute function of the function control action
 *
 * @param _control_action_function	the control_action_function_t
 * @param port											pointer to the control port to which the command should be sent
 */
static void control_action_function_execute(
		void* const _control_action_function, const control_port_t* port);

void control_action_function_init(
		control_action_function_t* const control_action_function) {
	control_action_simple_init(
			(control_action_simple_t*) control_action_function);

	control_action_function->execute_simple =
			control_action_function->super.super.execute;
	control_action_function->super.super.execute =
			control_action_function_execute;

	int i, f;
	for (i = 0; i < CONTROL_ACTION_SIMPLE_MAX_COMMANDS; i++) {
		for (f = 0; f < CONTROL_ACTION_SIMPLE_MAX_PARAMS_PER_COMMAND; f++) {
			control_action_function->function_list[i][f] = 0;
		}
	}
}

static void control_action_function_execute(
		void* const _control_action_function, const control_port_t* port) {
	control_action_function_t* control_action_function =
			(control_action_function_t*) _control_action_function;

	//calculate next values
	control_action_simple_t* control_action_simple =
			(control_action_simple_t*) _control_action_function;
	int i;
	for (i = 0; i < control_action_simple->configured_commands; i++) {
		int f;
		for (f = 0; f < control_action_simple->configured_parameters[i]; f++) {
			if (control_action_function->function_list[i][f]) {
				function_gen_t* fg = control_action_function->function_list[i][f];
				control_action_function->super.command_list[i][f].value = fg->next_val(
						fg);
			}
		}

	}

	//execute control action
	control_action_function->execute_simple(_control_action_function, port);
}

void control_action_function_add_function_paramter(
		control_action_function_t* const control_action_function,
		control_parameter_t* parameter, function_gen_t* function) {
	control_action_simple_t* control_action_simple =
			(control_action_simple_t*) control_action_function;
	control_action_simple_add_paramter(control_action_simple, parameter);

	unsigned int cur_command = control_action_simple->configured_commands - 1;
	unsigned int cur_parameter =
			control_action_simple->configured_parameters[cur_command] - 1;

	control_action_function->function_list[cur_command][cur_parameter] = function;
}

void control_action_function_add_function_paramter_exp(
		control_action_function_t* const control_action_function,
		parameter_type_t type, function_gen_t* function) {
	control_parameter_t p = { type, 0 };
	control_action_function_add_function_paramter(control_action_function, &p,
			function);
}
