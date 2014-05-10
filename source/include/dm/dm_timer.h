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
#include "control_port.h"
#include "datastream_object.h"

/**
 * @brief struct describing a timer module
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/

	const control_port_t* control_out; /**< control output port */

	timer_regs_t* timer; /**< pointer to the timer peripheral */
	compare_regs_t* compare; /**< pointer to the compare peripheral */
} dm_timer_t;

/**
 * @brief timer init function
 *
 * initializes the timer, should be called before using the module
 *
 * @param	timer					pointer to the timer
 * @param	interval			the interval of the timer in ms (max 25bit)
 * @param timer_regs 		pointer to a timer peripheral
 * @param compare_regs	pointer to a compare peripheral
 */
void dm_timer_init(dm_timer_t* const timer, const uint36_t interval,
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

#endif
