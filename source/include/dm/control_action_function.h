/**
 * @file control_action_function.h
 * @brief control action which generates the next value of a parameter by a function
 *
 * This can be used for example as an action performed on a trigger or timer event.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef CONTROL_ACTION_FUNCTION_H_
#define CONTROL_ACTION_FUNCTION_H_

#include <fpga-log/dm/control_action_simple.h>
#include <fpga-log/dm/function_gen.h>

/**
 * @brief struct defining a function control action
 */
typedef struct {
	control_action_simple_t super; /**< super-"class": control_action_simple_t */

	void (*execute_simple)(void* const control_action, const control_port_t* port);

	function_gen_t* function_list[CONTROL_ACTION_SIMPLE_MAX_COMMANDS][CONTROL_ACTION_SIMPLE_MAX_PARAMS_PER_COMMAND];
} control_action_function_t;

/**
 * @brief control_action_function init function
 *
 * @param	control_action_function	pointer to the control_action_function struct
 */
void control_action_function_init(
		control_action_function_t* const control_action_function);

void control_action_function_add_function_paramter(
		control_action_function_t* const control_action_function,
		control_parameter_t* parameter, function_gen_t* function);

void control_action_function_add_function_paramter_exp(
		control_action_function_t* const control_action_function,
		parameter_type_t type, function_gen_t* function);

#endif
