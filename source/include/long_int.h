/**
 * @file long_int.h
 * @brief long int support functions
 *
 * These functions are needed to fix some problems in the SpartanMC long int support.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */


/**
 * @brief casts a unsigned int to a unsigned long int
 *
 * This function is only needed because of a compiler bug.
 * Without explicitly using this function subreg optimization will fail
 * and calculation results could be wrong.
 *
 * Problem occured on GNU C (GCC) version 4.4.5 (spartanmc).
 * Verify if still necessary on newer versions.
 *
 * @param i value to cast
 * @return casted value
 */
unsigned long int cast_to_ulong(unsigned int i);

/**
 * @brief multiplies unsigned 34-bit value with a unsigned 17-bit value, result is truncated to 34-bit
 *
 * The SpartanMC core offers only a signed multiply so this function uses 17-bit values to make
 * sure the operands are positive. For most use cases this is no problem since values will be 16/32 bit.
 *
 * @param a_34 	34-bit operand
 * @param b_17	17-bit operand
 * @return	multiplication result, truncated to 34-bit
 */
unsigned long int mul34_17(unsigned long int a_34, unsigned int b_17);
