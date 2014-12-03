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

typedef enum {
	EVENT_TIMER_NORMAL, EVENT_TIMER_PERIODIC,
} dm_event_timer_mode;

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

	event_timer_event_t event_list[DM_EVENT_TIMER_MAX_EVENTS]; /**< event list */
	int first_event; /**< first element of event list */

	dm_event_timer_mode mode; /**< event timer mode (normal, periodic, ...) */
	int peridic_next; /**< next element of event list in periodic mode */
} dm_event_timer_t;

/**
 * @brief event timer init function
 *
 * Initializes the timer, should be called before using the module.
 *
 * @param	event_timer			pointer to the timer
 * @param mode 						mode of the event timer
 * @param timer_regs 			pointer to a timer peripheral
 * @param compare_regs		pointer to a compare peripheral
 */
void dm_event_timer_init(dm_event_timer_t* const event_timer,
		dm_event_timer_mode mode, timer_regs_t* const timer_regs,
		compare_regs_t* const compare_regs);

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
