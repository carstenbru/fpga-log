/**
 * @file function_gen.h
 * @brief function generator object
 *
 * This can be used for example as function for a control_action_function
 * Use this as super-"class" for function implementations.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef FUNCTION_GEN_H_
#define FUNCTION_GEN_H_

/**
 * @brief struct defining a function generator object
 *
 * Use this as super-"class" for more function generators.
 */
typedef struct {
	/**
	 * @brief generates the next value (sample) of the function
	 *
	 * @param function_gen	pointer to this struct
	 * @return							the next sample
	 */
	int (*next_val)(void* const function_gen);
} function_gen_t;

#endif
