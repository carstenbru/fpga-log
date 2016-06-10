/**
 * @file dm_timer.c
 * @brief timer module
 *
 * This module can be used to generate periodic measurements.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/dm_timer.h>

#include <fpga-log/peripheral_funcs/timer_funcs.h>
#include <fpga-log/peripheral_funcs/compare_funcs.h>

/**
 * @brief timer update function
 *
 * @param	_timer	pointer to the timer
 */
static void dm_timer_update(void* const _timer);

void dm_timer_init(dm_timer_t* const timer, const uint36_t interval,
		const uint36_t delay, control_action_t* const control_action,
		timer_regs_t* const timer_regs, compare_regs_t* const compare_regs) {
	datastream_object_init(&timer->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	timer->super.update = dm_timer_update;

	timer->control_out = &control_port_dummy;
	timer->control_action = control_action;

	timer->timer = timer_regs;
	timer->compare = compare_regs;

	timer->interval = interval;

	timer_enable(timer_regs);
	if (delay == 0) {
		dm_timer_set_interval(timer, interval);
		timer->start_delay = 0;
	} else {
		dm_timer_set_interval(timer, delay);
		timer->start_delay = 1;
	}

	compare_init_mode_set(compare_regs, 0);
}

void dm_timer_set_interval(dm_timer_t* const timer, uint36_t interval) {
	timer_set_interval_ms(timer->timer, interval);
}

void dm_timer_set_control_out(dm_timer_t* const timer,
		const control_port_t* const control_in) {
	timer->control_out = control_in;
}

static void dm_timer_update(void* const _timer) {
	dm_timer_t* timer = (dm_timer_t*) _timer;

	if (compare_check_and_reset_flag(timer->compare)) {
		if (timer->start_delay) {
			dm_timer_set_interval(timer, timer->interval);
			timer->start_delay = 0;
		}

		timer->control_action->execute(timer->control_action, timer->control_out);
	}
}

void dm_timer_set_control_action(dm_timer_t* const timer,
		control_action_t* const control_action) {
	timer->control_action = control_action;
}
