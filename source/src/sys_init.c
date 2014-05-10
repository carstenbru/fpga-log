/**
 * @file sys_init.c
 * @brief system initialization
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <system/peripherals.h>

#define CLOCK_SPEED 32000000

void sys_init(void) {
	/*
	 * timer starts with 0 -> subtract one
	 * reset from compare unit is two clock cycles -> subtract one
	 * => subtract two in total
	 */
	TIMER_2000HZ_COMPARE->CMP_DAT = (CLOCK_SPEED / 4000) - 2;
	TIMER_2000HZ_COMPARE->CMP_CTRL |= COMPARE_EN | COMPARE_EN_OUT | (COMPARE_MODE*7);
}
