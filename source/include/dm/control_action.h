/**
 * @file control_action.h
 * @brief control actions
 *
 * This can be used for example as an action performed on a trigger or timer event.
 * Use this as super-"class" for more complex conditions.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef CONTROL_ACTION_H_
#define CONTROL_ACTION_H_

#include <fpga-log/control_port.h>

/**
 * @brief struct defining a control action
 *
 * Use this as super-"class" for more complex actions.
 */
typedef struct {
	/**
	 * @brief executes the control action
	 *
	 * @param control_action	pointer to this struct
	 * @param port						pointer to the control port to which the command should be sent
	 */
	void (*execute)(void* const control_action, const control_port_t* port);
} control_action_t;

#endif
