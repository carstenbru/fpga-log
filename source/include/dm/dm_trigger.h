/**
 * @file dm_trigger.h
 * @brief trigger module for datastreams
 *
 * This module can be used to trigger a measurement (on the control-port)
 * on a specific condition on the data-port.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_TRIGGER_H_
#define DM_TRIGGER_H_

#include "data_port.h"
#include "control_port.h"
#include "dm/datastream_condition.h"

/**
 * @brief struct describing a datastream trigger
 */
typedef struct {
	data_port_t data_in; /**< data port, this can be set at a data output to direct the data stream to this trigger */
	const control_port_t* control_out; /**< control output port */

	datastream_condition_t* condition; /**< trigger condition defining when the trigger fires*/
} dm_trigger_t;

/**
 * @brief datastream trigger init function
 *
 * Initializes the trigger, should be called before using the trigger.
 * By default the trigger fires on every incoming package. This can be changed by setting
 * a different condition with @ref dm_trigger_set_condition function.
 *
 * @param	trigger		pointer to the datastream trigger
 */
void dm_trigger_init(dm_trigger_t* const trigger);

/**
 * @brief connects the control output port of a trigger to a given destination
 *
 * @param	trigger			pointer to the trigger
 * @param	control_in	the new control destination
 */
void dm_trigger_set_control_out(dm_trigger_t* const trigger,
		const control_port_t* const control_in);

/**
 * @brief sets a new trigger condition
 *
 * @param trigger		pointer to the trigger
 * @param condition	the new trigger condition
 */
void dm_trigger_set_condition(dm_trigger_t* const trigger,
		datastream_condition_t* const condition);


#endif
