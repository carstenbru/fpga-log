/**
 * @file timer_funcs.h
 * @brief functions to control SpartanMC's timer peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <peripherals/timer.h>

/**
 * @brief starts the timer (sets timer and prescaler enable bits)
 *
 * @param timer	pointer to the peripheral registers
 */
void timer_enable(timer_regs_t* const timer);

/**
 * @brief sets the prescaler and limit of a timer
 *
 * @param timer			pointer to the peripheral registers
 * @param prescaler	new prescaler value as power of two (e.g. 3 is 2^3=8)
 * @param limit			new limit value of the timer
 */
void timer_set_interval(timer_regs_t* const timer, const unsigned int prescaler,
		const unsigned int limit);

#endif
