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

typedef long int_ptr;

/**
 * @brief struct describing in- and out-pipes
 */
typedef struct {
	int in; /**< in pipe */
	int out; /**< out pipe */
} file_pipe_t;

/**
 * @brief struct describing a simulated timer
 */
typedef struct {
	struct timeval start; /**< start time of the timer */

	int prescaler; /**< prescaler value */
	int limit; /**< limit value */
} spmc_timer_t;

/**
 * @brief struct describing a simulated compare unit
 */
typedef struct {
	struct timeval last; /**< time of last compare check*/

	int comp_val; /**< compare value */
} spmc_compare_t;

/**
 * @brief array of the pipes simulating SpartanMC peripherals
 */
extern file_pipe_t pipes[PIPE_COUNT];

/**
 * @brief array of simulated timers
 */
extern spmc_timer_t sim_timers[TIMER_COUNT];

/**
 * @brief array of simulated compare units
 */
extern spmc_compare_t sim_compares[COMPARE_COUNT];

#endif

#endif
