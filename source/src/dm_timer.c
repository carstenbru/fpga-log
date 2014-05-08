/**
 * @file dm_timer.c
 * @brief timer module
 *
 * This module can be used to generate periodic measurements.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/dm_timer.h"

/**
 * @brief timer update function
 *
 * @param	_timer	pointer to the timer
 */
static void dm_timer_update(void* const _timer);

void dm_timer_init(dm_timer_t* const timer, timer_regs_t* const timer_regs,
		compare_regs_t* const compare_regs) {
	datastream_object_init(&timer->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	timer->super.update = dm_timer_update;

	timer->control_out = &control_port_dummy;

	timer->timer = timer_regs;
	timer->compare = compare_regs;

	timer_regs->limit = 62500;
	timer_regs->control |= TIMER_PRE_256 | TIMER_PRE_EN | TIMER_EN;

	compare_regs->CMP_DAT = 0;
	compare_regs->CMP_CTRL |= COMPARE_EN | (COMPARE_MODE*1);
}

void dm_timer_set_control_out(dm_timer_t* const timer,
		const control_port_t* const control_in) {
	timer->control_out = control_in;
}

static void dm_timer_update(void* const _timer) {
	dm_timer_t* timer = (dm_timer_t*) _timer;

	if (timer->compare->CMP_CTRL &  COMPARE_VAL_OUT) {
		timer->compare->CMP_CTRL &=  ~COMPARE_VAL_OUT; //reset val_out bit

		const control_port_t* out = timer->control_out;
		out->measure(out->parent);
	}
}

/*
 * TODO
 * -configurable interval
 * -replace timer with timer_rti? (perhaps set only flag in interrupt then)
 * -PC support
 */
