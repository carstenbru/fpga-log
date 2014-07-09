/**
 * @file simple_float.h
 * @brief simple floating point implementation (base 10)
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/simple_float.h>

void simple_float_b10_init(simple_float_b10_t* const simple_float_b10,
		coefficient_t coefficient, int exponent) {
	simple_float_b10->coefficient = coefficient;
	simple_float_b10->exponent = exponent;
}
