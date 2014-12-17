/**
 * @file src_PC/timer_funcs.c
 * @brief native PC functions to simulate SpartanMC's timer peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "peripheral_funcs/timer_funcs.h"

#include <stdlib.h>
#include <sys/time.h>

#include <fpga-log/pc_compatibility.h>

spmc_timer_t sim_timers[TIMER_COUNT];

void timer_enable(timer_regs_t* const timer) {
	gettimeofday(&(sim_timers[(int_ptr) timer].start), NULL);
}

void timer_set_interval(timer_regs_t* const timer, const unsigned int prescaler,
		const unsigned int limit) {
	sim_timers[(int_ptr) timer].prescaler = prescaler;
	sim_timers[(int_ptr) timer].limit = limit;
}

void timer_set_interval_ms(timer_regs_t* const timer, uint36_t interval) {
	int prescale = 0;
	while ((interval > 262144) && (prescale < 8)) {
		prescale++;
		interval >>= 1;
	}

	timer_set_interval(timer, prescale, --interval);
}
