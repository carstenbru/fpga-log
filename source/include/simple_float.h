/**
 * @file simple_float.h
 * @brief simple floating point implementation (base 10)
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef SIMPLE_FLOAT_H_
#define SIMPLE_FLOAT_H_

typedef long coefficient_t;

/**
 * @brief simple floating point data structure (base 10)
 */
typedef struct {
	coefficient_t coefficient; /**< coefficient of the floating point number as fixed point number with 4 decimal places */
	int exponent; /**< exponent of the floating point number to base 10 */
} simple_float_b10_t;

/**
 * @brief simple float init function
 *
 * @param simple_float_b10	pointer to the simple float struct
 * @param coefficient				coefficient of the float
 * @param exponent					exponent of the float
 */
void simple_float_b10_init(simple_float_b10_t* const simple_float_b10,
		coefficient_t coefficient, int exponent);

#endif
