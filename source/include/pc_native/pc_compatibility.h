/**
 * @file pc_compatibility.h
 * @brief defintions needed to run and simulate the code natively on a PC
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef PC_COMPATIBILITY_H_
#define PC_COMPATIBILITY_H_

#ifdef PC_NATIVE

#include <sys/time.h>

#include <peripherals/compare.h>
#include <peripherals/timer.h>

#include "pc_native/peripherals.h"

/**
 * @brief struct describing in- and out-pipes
 */
typedef struct {
	int in; /**< in pipe */
	int out; /**< out pipe */
} file_pipe_t;

typedef struct {
	struct timeval start;

	int prescaler;
	int limit;
} spmc_timer_t;

typedef struct {
	struct timeval last;

	int comp_val;
} spmc_compare_t;

extern compare_regs_t* TIMER_2000HZ_COMPARE;
/**
 * @brief array of the pipes simulating SpartanMC peripherals
 */
extern file_pipe_t pipes[PIPE_COUNT];

extern spmc_timer_t sim_timers[TIMER_COUNT];
extern spmc_compare_t sim_compares[COMPARE_COUNT];

#endif

#endif
