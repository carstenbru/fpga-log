/**
 * @file compare_funcs.h
 * @brief functions to control SpartanMC's compare peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef COMPARE_H_
#define COMPARE_H_

#include <peripherals/compare.h>

/**
 * @brief initializes (enables) the compare unit in set-output-mode
 *
 * @param compare		pointer to the compare peripheral
 * @param comp_val	new compare value
 */
void compare_init_mode_set(compare_regs_t* const compare,
		const unsigned int comp_val);

/**
 * @brief checks weather the comparator output bit is set and resets it
 *
 * @param compare	pointer to the compare peripheral
 * @return	1 if the flag was set, otherwise 0
 */
int compare_check_and_reset_flag(compare_regs_t* const compare);

#endif
