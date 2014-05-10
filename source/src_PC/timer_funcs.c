/**
 * @file timer_funcs.c
 * @brief native PC functions to simulate SpartanMC's timer peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "peripherals/timer_funcs.h"

#include <stdlib.h>
#include <sys/time.h>

#include "pc_native/pc_compatibility.h"
#include "pc_native/peripherals.h"

spmc_timer_t sim_timers[TIMER_COUNT];

void timer_enable(timer_regs_t* const timer) {
	gettimeofday(&(sim_timers[(int) timer].start), NULL);
}

void timer_set_interval(timer_regs_t* const timer, const unsigned int prescaler,
		const unsigned int limit) {
	sim_timers[(int) timer].prescaler = prescaler;
	sim_timers[(int) timer].limit = limit;
}
