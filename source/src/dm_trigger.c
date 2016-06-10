/**
 * @file dm_trigger.c
 * @brief trigger module for datastreams
 *
 * This module can be used to trigger a measurement (on the control-port)
 * on a specific condition on the data-port.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/dm_trigger.h>

/**
 * @brief incoming data function of the trigger
 *
 * @param	_trigger	pointer to the trigger
 * @param	package		the incoming data package
 */
static void dm_trigger_new_data(void* const _trigger,
		const data_package_t* const package);

void dm_trigger_init(dm_trigger_t* const trigger,
		control_action_t* const control_action) {
	trigger->control_out = &control_port_dummy;
	trigger->control_action = control_action;

	trigger->data_in = data_port_dummy;
	trigger->data_in.parent = (void*) trigger;
	trigger->data_in.new_data = dm_trigger_new_data;

	trigger->condition = &datastream_condition_always_true;
}

data_port_t* dm_trigger_get_data_in(dm_trigger_t* const trigger) {
	return &trigger->data_in;
}

void dm_trigger_set_control_out(dm_trigger_t* const trigger,
		const control_port_t* const control_in) {
	trigger->control_out = control_in;
}

void dm_trigger_set_condition(dm_trigger_t* const trigger,
		datastream_condition_t* const condition) {
	trigger->condition = condition;
}

static void dm_trigger_new_data(void* const _trigger,
		const data_package_t* const package) {
	dm_trigger_t* trigger = (dm_trigger_t*) _trigger;

	datastream_condition_t* const cond = trigger->condition;
	if (cond->is_fulfilled(cond, package)) {
		trigger->control_action->execute(trigger->control_action,
				trigger->control_out);
	}
}

void dm_trigger_set_control_action(dm_trigger_t* const _trigger,
		control_action_t* const control_action) {
	_trigger->control_action = control_action;
}
