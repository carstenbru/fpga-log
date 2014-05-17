/**
 * @file dm_timer.c
 * @brief timer module
 *
 * This module can be used to generate periodic measurements.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/dm_timer.h"

#include "peripheral_funcs/timer_funcs.h"
#include "peripheral_funcs/compare_funcs.h"

/**
 * @brief timer update function
 *
 * @param	_timer	pointer to the timer
 */
static void dm_timer_update(void* const _timer);

void dm_timer_init(dm_timer_t* const timer, const uint36_t interval,
		timer_regs_t* const timer_regs, compare_regs_t* const compare_regs) {
	datastream_object_init(&timer->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	timer->super.update = dm_timer_update;

	timer->control_out = &control_port_dummy;

	timer->timer = timer_regs;
	timer->compare = compare_regs;

	timer_enable(timer_regs);
	dm_timer_set_interval(timer, interval);

	compare_init_mode_set(compare_regs, 0);
}

void dm_timer_set_interval(dm_timer_t* const timer, uint36_t interval) {
	int prescale = 0;
	while ((interval > 262144) && (prescale < 8)) {
		prescale++;
		interval >>= 1;
	}

	timer_set_interval(timer->timer, prescale, interval);
}

void dm_timer_set_control_out(dm_timer_t* const timer,
		const control_port_t* const control_in) {
	timer->control_out = control_in;
}

static void dm_timer_update(void* const _timer) {
	dm_timer_t* timer = (dm_timer_t*) _timer;

	if (compare_check_and_reset_flag(timer->compare)) {
		const control_port_t* out = timer->control_out;
		out->measure(out->parent);
	}
}
