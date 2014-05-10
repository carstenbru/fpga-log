/**
 * @file compare_funcs.c
 * @brief native PC functions to simulate SpartanMC's compare peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "peripherals/compare_funcs.h"

#include <sys/time.h>
#include <stdlib.h>

#include "pc_native/peripherals.h"
#include "pc_native/pc_compatibility.h"

spmc_compare_t sim_compares[COMPARE_COUNT];

void compare_init_mode_set(compare_regs_t* const compare,
		const unsigned int comp_val) {
	//compare->CMP_DAT = comp_val;
	//compare->CMP_CTRL |= COMPARE_EN | (COMPARE_MODE * 1);
	sim_compares[(int) compare].comp_val = comp_val;
}

/**
 * @brief returns the time difference in ms between two timevals
 *
 * @param a	first timeval (later one)
 * @param b second timeval (earlier one)
 * @return	time difference in ms
 */
long timediff_ms(struct timeval* a, struct timeval* b) {
	return (a->tv_sec - b->tv_sec) * 1000 + (a->tv_usec - b->tv_usec) / 1000;
}

int compare_check_and_reset_flag(compare_regs_t* const compare) {
	spmc_compare_t* comp = &sim_compares[(int) compare];
	spmc_timer_t* tim = &sim_timers[(int) compare];

	struct timeval now;
	gettimeofday(&now, NULL);

	long ticks = timediff_ms(&now, &comp->last);
	ticks /= (1 << tim->prescaler);

	if (ticks == 0)
		return 0;

	comp->last = now;
	if (ticks > tim->limit) {  //if timer made more ticks than limit compare is always true
		return 1;
	} else {
		int tim_val = timediff_ms(&comp->last, &tim->start);
		tim_val /= (1 << tim->prescaler);
		tim_val += ticks;
		tim_val %= tim->limit;

		if (comp->comp_val > tim_val) {
			return (tim_val + ticks) > comp->comp_val;
		} else {
			return (tim_val - ticks) < comp->comp_val;
		}
	}
}
