/**
 * @file control_action_simple.h
 * @brief simple control action which sends always the same commands to its control port
 *
 * This can be used for example as an action performed on a trigger or timer event.
 * Use this as super-"class" for more complex conditions.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef CONTROL_ACTION_SIMPLE_H_
#define CONTROL_ACTION_SIMPLE_H_

#include "control_action.h"

/**
 * @brief maximum number of commands the control action can consist of
 */
#define CONTROL_ACTION_SIMPLE_MAX_COMMANDS 8
/**
 * @brief maximum number of parameters a command in the control action can consist of
 */
#define CONTROL_ACTION_SIMPLE_MAX_PARAMS_PER_COMMAND 5

/**
 * @brief struct defining a simple control action
 */
typedef struct {
	control_action_t super; /**< super-"class": control_action_t */

	unsigned int configured_commands; /**< amount of actual configured commands */
	unsigned int configured_parameters[CONTROL_ACTION_SIMPLE_MAX_COMMANDS]; /**< amount of actual configured parameters in each command */
	control_parameter_t command_list[CONTROL_ACTION_SIMPLE_MAX_COMMANDS][CONTROL_ACTION_SIMPLE_MAX_PARAMS_PER_COMMAND]; /**< array representing the commands send by the control action */
} control_action_simple_t;

/**
 * @brief control_action_simple init function
 *
 * @param	control_action_simple	pointer to the control_action_simple struct
 */
void control_action_simple_init(
		control_action_simple_t* const control_action_simple);

/**
 * @brief adds a command to the control_action_simple
 *
 * The parameters have to be defined with @ref control_action_simple_add_paramter after this function call.
 *
 * @param	control_action_simple	pointer to the control_action_simple struct
 */
void control_action_simple_add_command(
		control_action_simple_t* const control_action_simple);

/**
 * @brief adds a parameter to the last command
 *
 * See also @ref control_action_simple_add_paramter_exp
 *
 * @param	control_action_simple	pointer to the control_action_simple struct
 * @param parameter							the parameter which should be added to the command, value is copied
 */
void control_action_simple_add_paramter(
		control_action_simple_t* const control_action_simple,
		control_parameter_t* paramter);

/**
 * @brief adds a parameter to the last command
 *
 * See also @ref control_action_simple_add_paramter
 *
 * @param	control_action_simple	pointer to the control_action_simple struct
 * @param type									the type of the new parameter
 * @param value									the value of the new parameter
 */
void control_action_simple_add_paramter_exp(
		control_action_simple_t* const control_action_simple, int type, int value);

#endif
