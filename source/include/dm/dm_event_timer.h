/**
 * @file dm_event_timer.h
 * @brief event timer module
 *
 * This module can be used to generate control commands with various timings.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_EVENT_TIMER_H_
#define DM_EVENT_TIMER_H_

#include <peripherals/timer.h>
#include <peripherals/compare.h>
#include <stdint.h>
#include <fpga-log/control_port.h>
#include <fpga-log/datastream_object.h>
#include <fpga-log/dm/control_action.h>

#define DM_EVENT_TIMER_MAX_EVENTS 8

typedef struct {
	uint36_t delay;
	control_action_t* control_action;

	int next;
} event_timer_event_t;

/**
 * @brief struct describing a timer module
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/

	const control_port_t* control_out; /**< control output port */

	timer_regs_t* timer; /**< pointer to the timer peripheral */
	compare_regs_t* compare; /**< pointer to the compare peripheral */

	event_timer_event_t event_list[DM_EVENT_TIMER_MAX_EVENTS];
	int first_event;
} dm_event_timer_t;

/**
 * @brief event timer init function
 *
 * Initializes the timer, should be called before using the module.
 *
 * @param	event_timer			pointer to the timer
 * @param timer_regs 			pointer to a timer peripheral
 * @param compare_regs		pointer to a compare peripheral
 */
void dm_event_timer_init(dm_event_timer_t* const event_timer,
		timer_regs_t* const timer_regs, compare_regs_t* const compare_regs);

/**
 * @brief connects the control output port of a event timer to a given destination
 *
 * @param	event_timer	pointer to the timer
 * @param	control_in	the new control destination
 */
void dm_event_timer_set_control_out(dm_event_timer_t* const event_timer,
		const control_port_t* const control_in);

void dm_event_timer_add_event(dm_event_timer_t* const event_timer,
		uint36_t delay, control_action_t* const control_action);

#endif
