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
#include <peripherals/timestamp_gen.h>

#include "pc_peripherals.h"

unsigned int timestamp_gen_not_empty(timestamp_gen_regs_t* timestamp_gen);
void timestamp_gen_generate_software_timestamp(
		timestamp_gen_regs_t* timestamp_gen, unsigned int id);


#define TIMESTAMP_GEN_NOT_EMPTY(tsgen) (timestamp_gen_not_empty(tsgen))
#define TIMESTAMP_GEN_GENERATE_SOFTWARE_TIMESTAMP(tsgen, id) (timestamp_gen_generate_software_timestamp(tsgen, id))

#define MAX_SOFTWARE_TIMESTAMPS 64

#define TIMER_2000HZ_COMPARE 0
#define TIMESTAMP_GEN 10

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

/**
 * @brief fifo/pipe read function
 *
 * @param fifo 	number of the fifo to read
 * @param data	pointer to char where data should be stored
 * @return 1 on success, 0 on eof and -1 on error
 */
int fifo_read(unsigned int fifo, unsigned char* data);

/**
 * @brief fifo/pipe write function
 *
 * @param fifo	number of the fifo to write
 * @param data	data to write
 * @return	1 on success
 */
int fifo_write(unsigned int fifo, unsigned char data);

#else
#define TIMESTAMP_GEN_NOT_EMPTY(tsgen) (tsgen->status)
#define TIMESTAMP_GEN_GENERATE_SOFTWARE_TIMESTAMP(tsgen, id) (tsgen->control = id)
#endif

#endif
