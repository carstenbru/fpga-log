/**
 * @file sys_init.h
 * @brief system initialization
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef SYS_INIT_H_
#define SYS_INIT_H_

#include <peripherals/timestamp_gen.h>
#include <peripherals/compare.h>

/**
 * @brief initializes the system
 *
 * @param periperhal_clock			the peripheral clock of the system
 * @param tsgen									timestamp generator peripheral
 */
void sys_init(unsigned long int periperhal_clock, timestamp_gen_regs_t* tsgen);

/**
 * @brief returns the peripheral clock of the system
 *
 * @return	the peripheral clock
 */
unsigned long int get_peri_clock(void);

/**
 * @brief returns the timestamp generator peripheral of the system
 *
 * @return	the timestamp generator
 */
timestamp_gen_regs_t* get_timestamp_gen(void);

#endif
