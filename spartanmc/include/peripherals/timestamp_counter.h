/**
 * @file timestamp_counter.h
 * @brief timestamp generator counter peripheral registers
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef __TIMESTAMP_COUNTER_H
#define __TIMESTAMP_COUNTER_H

#include "timestamp_gen.h"

/**
 * @brief timestamp counter registers
 */
typedef struct {
	volatile timestamp_t timestamp; /**< timestamp counters as timestamp struct, see struct @ref timestamp_t */
} timestamp_counter_regs_t;

#endif
