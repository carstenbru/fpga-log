/**
 * @file sys_init.c
 * @brief system initialization
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/sys_init.h>

unsigned long int peri_clock;
timestamp_gen_regs_t* timerstamp_gen;

void sys_init(unsigned long int periperhal_clock, timestamp_gen_regs_t* tsgen) {
	timerstamp_gen = tsgen;
	peri_clock = periperhal_clock;

	timerstamp_gen->control = TIMESTAMP_GEN_FIFO_CLEAR;
}

unsigned long int get_peri_clock(void) {
	return peri_clock;
}

timestamp_gen_regs_t* get_timestamp_gen(void) {
	return timerstamp_gen;
}
