/**
 * @file compare_funcs.c
 * @brief functions to control SpartanMC's compare peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "peripherals/compare_funcs.h"

void compare_init_mode_set(compare_regs_t* const compare,
		const unsigned int comp_val) {
	compare->CMP_DAT = comp_val;
	compare->CMP_CTRL |= COMPARE_EN | (COMPARE_MODE * 1);
}

int compare_check_and_reset_flag(compare_regs_t* const compare) {
	if (compare->CMP_CTRL & COMPARE_VAL_OUT) {
		compare->CMP_CTRL &= ~COMPARE_VAL_OUT; //reset val_out bit
		return 1;
	}
	return 0;
}
