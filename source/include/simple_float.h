/**
 * @file simple_float.h
 * @brief simple floating point implementation (base 10)
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

/**
 * @brief simple floating point data structure (base 10)
 */
typedef union {
	long int long_comp; /**< representation of the float as long to make use of the long integer functions */
	struct {
		unsigned int coefficient; /**< coefficient of the floating point number as fixed point number with 4 decimal places */
		int exponent; /**< exponent of the floating point number to base 10 */
	};
} simple_float_b10_t;
