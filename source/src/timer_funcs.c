/**
 * @file src/timer_funcs.c
 * @brief functions to control SpartanMC's timer peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/peripheral_funcs/timer_funcs.h>

void timer_enable(timer_regs_t* const timer) {
	timer->control |= TIMER_PRE_EN | TIMER_EN;
}

void timer_set_interval(timer_regs_t* const timer, const unsigned int prescaler,
		const unsigned int limit) {
	timer->limit = limit;
	int control = timer->control;

	control &= ~28;  //reset prescaler bits
	control |= (prescaler << 2);  //set prescaler value

	timer->control = control;
}

void timer_set_interval_ms(timer_regs_t* const timer, uint36_t interval) {
	int prescale = 0;
	while ((interval > 262144) && (prescale < 8)) {
		prescale++;
		interval >>= 1;
	}

	timer_set_interval(timer, prescale, --interval);
}
