/**
 * @file pc_compatibility.h
 * @brief defintions needed to run and simulate the code natively on a PC
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef PC_COMPATIBILITY_H_
#define PC_COMPATIBILITY_H_

#ifdef PC_NATIVE

#include "peripheral_pipes.h"

/**
 * @brief struct describing in- and out-pipes
 */
typedef struct {
	int in;		/**< in pipe */
	int out;	/**< out pipe */
} file_pipe;

/**
 * @brief initialization function for native PC compatibility code
 */
void pc_native_init(void);

#else

/**
 * @brief initialization function for native PC compatibility code
 */
void pc_native_init(void) {}

#endif

#endif
