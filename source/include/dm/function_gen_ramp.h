/**
 * @file function_gen_ramp.h
 * @brief ramp function generator object
 *
 * This can be used for example as function for a control_action_function
 * Use this as super-"class" for function implementations.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef FUNCTION_GEN_RAMP_H_
#define FUNCTION_GEN_RAMP_H_

#include <fpga-log/dm/function_gen.h>

/**
 * @brief struct defining a sawtooth function generator object
 *
 */
typedef struct {
	function_gen_t super; /**< super-"class": function_gen_t */

	int start_val;
	int end_val;
	unsigned int samples;

	int cur_sample;
} function_gen_ramp_t;

void function_gen_ramp_init(function_gen_ramp_t* const function_gen_ramp,
		int start_val, int end_val, unsigned int samples);

#endif
