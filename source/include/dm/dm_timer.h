/**
 * @file dm_timer.h
 * @brief timer module
 *
 * This module can be used to generate periodic measurements.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_TIMER_H_
#define DM_TIMER_H_

#include <peripherals/timer.h>
#include <peripherals/compare.h>
#include <stdint.h>
#include <fpga-log/control_port.h>
#include <fpga-log/datastream_object.h>
#include <fpga-log/dm/control_action.h>

/**
 * @brief struct describing a timer module
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/

	const control_port_t* control_out; /**< control output port */
	control_action_t* control_action; /**< executed control action */

	timer_regs_t* timer; /**< pointer to the timer peripheral */
	compare_regs_t* compare; /**< pointer to the compare peripheral */

	unsigned char start_delay; /**< start delay in progress */
	uint36_t interval; /**< timer interval */
} dm_timer_t;

/**
 * @brief timer init function
 *
 * Initializes the timer, should be called before using the module.
 *
 * By default the timer sends a measure command to its control output. This can be changed by setting
 * a different control action with @ref dm_timer_set_control_action function.
 *
 * @param	timer						pointer to the timer
 * @param	interval				the interval of the timer in ms (max 25bit)
 * @param delay						delay at startup in ms (max 25bit)
 * @param control_action	the control_action which is executed by the timer
 * @param timer_regs 			pointer to a timer peripheral
 * @param compare_regs		pointer to a compare peripheral
 */
void dm_timer_init(dm_timer_t* const timer, const uint36_t interval,
		const uint36_t delay, control_action_t* const control_action,
		timer_regs_t* const timer_regs, compare_regs_t* const compare_regs);

/**
 * @brief sets the timer interval
 *
 * @param timer			pointer to the timer
 * @param interval	the new interval of the timer in ms (max 25bit)
 */
void dm_timer_set_interval(dm_timer_t* const timer, uint36_t interval);

/**
 * @brief connects the control output port of a timer to a given destination
 *
 * @param	timer				pointer to the timer
 * @param	control_in	the new control destination
 */
void dm_timer_set_control_out(dm_timer_t* const timer,
		const control_port_t* const control_in);

/**
 * @brief sets the control action to execute by a timer
 *
 * @param timer						pointer to the timer
 * @param control_action	the new control action
 */
void dm_timer_set_control_action(dm_timer_t* const timer,
		control_action_t* const control_action);

#endif
