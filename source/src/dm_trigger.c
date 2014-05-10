/**
 * @file dm_trigger.h
 * @brief trigger module for datastreams
 *
 * This module can be used to trigger a measurement (on the control-port)
 * on a specific condition on the data-port.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/dm_trigger.h"

/**
 * @brief incoming data function of the trigger
 *
 * @param	_trigger	pointer to the trigger
 * @param	package		the incoming data package
 */
static void dm_trigger_new_data(void* const _trigger,
		const data_package_t* const package);

void dm_trigger_init(dm_trigger_t* const trigger) {
	trigger->control_out = &control_port_dummy;

	trigger->data_in = data_port_dummy;
	trigger->data_in.parent = (void*) trigger;
	trigger->data_in.new_data = dm_trigger_new_data;

	trigger->condition = &datastream_condition_always_true;
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
		const control_port_t* out = trigger->control_out;
		out->measure(out->parent);
	}
}
