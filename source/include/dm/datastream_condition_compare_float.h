/**
 * @file datastream_condition_compare_float.h
 * @brief datastream compare conditions for simple_flaot_b10 (equal, smaller, greater,..)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAM_CONDITION_COMPARE_FLOAT_H_
#define DATASTREAM_CONDITION_COMPARE_FLOAT_H_

#include "dm/datastream_condition.h"
#include "simple_float.h"

/**
 * @brief struct defining a compare condition on long or simple_float_b10
 */
typedef struct {
	datastream_condition_t super; /**< super-"class": datastream_condition_t */

	/**
	 * compare function which does the actual compare
	 *
	 * @param val	the new value
	 * @param ref	the reference value
	 * @return 1 if compare is true, otherwise 0
	 */
	int (*compare_func)(const simple_float_b10_t val,
			const simple_float_b10_t ref);
	simple_float_b10_t compare_value; /**< value which is used for the comparison */
} datastream_condition_compare_float_t;

/**
 * @brief initializes the compare condition
 *
 * @param cond			pointer to the compare condition to initialize
 * @param comp_func	compare function which compares new value with the reference value
 * @param value			new compare value
 */
void datastream_condition_compare_compare_float_init(
		datastream_condition_compare_float_t* const cond,
		int (*compare_func)(const simple_float_b10_t val,
				const simple_float_b10_t ref), const simple_float_b10_t value);

/**
 * @brief sets a new compare value of the condition
 *
 * @param cond	the condition to modify
 * @param value	the new value
 */
void datastream_condition_compare_compare_float_set_compare_value(
		datastream_condition_compare_float_t* const cond,
		const simple_float_b10_t value);

/*
 * different compare functions
 */
int smaller_float(const simple_float_b10_t val, const simple_float_b10_t ref);
int greater_float(const simple_float_b10_t val, const simple_float_b10_t ref);
int equal_float(const simple_float_b10_t val, const simple_float_b10_t ref);
int not_equal_float(const simple_float_b10_t val, const simple_float_b10_t ref);
int smaller_equal_float(const simple_float_b10_t val,
		const simple_float_b10_t ref);
int greater_equal_float(const simple_float_b10_t val,
		const simple_float_b10_t ref);

#endif
