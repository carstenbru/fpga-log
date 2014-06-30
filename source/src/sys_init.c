/**
 * @file sys_init.c
 * @brief system initialization
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/sys_init.h>

unsigned long int peri_clock;
timestamp_gen_regs_t* timerstamp_gen;

void sys_init(unsigned long int periperhal_clock,
		compare_regs_t* timer_2000hz_compare, timestamp_gen_regs_t* tsgen) {
	timerstamp_gen = tsgen;
	peri_clock = periperhal_clock;
	periperhal_clock /= cast_to_ulong(2000);

	/*
	 * timer starts with 0 -> subtract one
	 * reset from compare unit is two clock cycles -> subtract one
	 * => subtract two in total
	 */
	timer_2000hz_compare->CMP_DAT = periperhal_clock - 2;
	timer_2000hz_compare->CMP_CTRL |= COMPARE_EN | COMPARE_EN_OUT
			| (COMPARE_MODE * 7);
	timerstamp_gen->control = TIMESTAMP_GEN_FIFO_CLEAR;
}

unsigned long int get_peri_clock(void) {
	return peri_clock;
}

timestamp_gen_regs_t* get_timestamp_gen(void) {
	return timerstamp_gen;
}
