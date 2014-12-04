/**
 * @file function_gen_ramp.c
 * @brief ramp function generator object
 *
 * This can be used for example as function for a control_action_function
 * Use this as super-"class" for function implementations.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/function_gen_ramp.h>
#include <fpga-log/long_int.h>

static int function_gen_ramp_next_val(void* _function_gen_ramp) {
	function_gen_ramp_t* function_gen_ramp =
			(function_gen_ramp_t*) _function_gen_ramp;

	unsigned long calc;
	calc = (function_gen_ramp->end_val - function_gen_ramp->start_val);
	calc *= function_gen_ramp->cur_sample;
	calc /= cast_to_ulong(function_gen_ramp->samples);
	int ret = function_gen_ramp->start_val + calc;  //TODO overflow/long int problems?

	function_gen_ramp->cur_sample++;
	if (function_gen_ramp->cur_sample > function_gen_ramp->samples)
		function_gen_ramp->cur_sample = 0;
	return ret;
}

void function_gen_ramp_init(function_gen_ramp_t* const function_gen_ramp,
		int start_val, int end_val, unsigned int samples) {
	function_gen_ramp->super.next_val = function_gen_ramp_next_val;

	function_gen_ramp->start_val = start_val;
	function_gen_ramp->end_val = end_val;
	if (samples > 1) {
		function_gen_ramp->samples = samples - 1;
	} else {
		function_gen_ramp->samples = 1;
	}
	function_gen_ramp->cur_sample = 0;
}
