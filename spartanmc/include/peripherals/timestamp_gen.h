/**
 * @file timestamp_gen.h
 * @brief timestamp generator peripheral registers
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef __TIMESTAMP_GEN_H
#define __TIMESTAMP_GEN_H

#define TIMESTAMP_GEN_NEXT_VAL		(1 << 16)
#define TIMESTAMP_GEN_FIFO_CLEAR	(1 << 17)

typedef struct {
	volatile union {
		unsigned long int lpt; /**< low precision timestamp as long */
		struct {
			unsigned int lpt_low; /**< low precision timestamp as uint, low value */
			unsigned int lpt_high; /**< low precision timestamp as uint, high value */
		};
	};
	volatile union {
		unsigned long int hpt; /**< high precision timestamp as long */
		struct {
			unsigned int hpt_low; /**< high precision timestamp as uint, low value */
			unsigned int hpt_high; /**< high precision timestamp as uint, high value */
		};
	};
} timestamp_t;

/**
 * @brief timestamp peripheral registers
 */
typedef struct {
	volatile timestamp_t timestamp; /**< timestamp values, see struct @ref timestamp_t */
	volatile unsigned int tsr; /**< timestamp source register */

	volatile unsigned int control; /**< control register */
	volatile unsigned int status; /**< status register */
} timestamp_gen_regs_t;

#endif
