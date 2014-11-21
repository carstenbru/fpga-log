/**
 * @file datastream_condition_compare_long.h
 * @brief datastream compare conditions for long (equal, smaller, greater,..)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAM_CONDITION_COMPARE_LONG_H_
#define DATASTREAM_CONDITION_COMPARE_LONG_H_

#include <fpga-log/dm/datastream_condition.h>

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
	int (*compare_func)(const long int val, const long int ref);
	long compare_value; /**< value which is used for the comparison */
} datastream_condition_compare_long_t;

/**
 * @brief initializes the compare condition
 *
 * @param cond					pointer to the compare condition to initialize
 * @param compare_func	compare function which compares new value with the reference value
 * @param value					new compare value
 */
void datastream_condition_compare_long_init(
		datastream_condition_compare_long_t* const cond,
		int (*compare_func)(const long int val, const long int ref),
		const long value);

/**
 * @brief sets a new compare value of the condition
 *
 * @param cond	the condition to modify
 * @param value	the new value
 */
void datastream_condition_compare_long_set_compare_value(
		datastream_condition_compare_long_t* const cond, const long value);

/*
 * different compare functions
 */
int smaller_long(const long int val, const long int ref);
int greater_long(const long int val, const long int ref);
int equal_long(const long int val, const long int ref);
int not_equal_long(const long int val, const long int ref);
int smaller_equal_long(const long int val, const long int ref);
int greater_equal_long(const long int val, const long int ref);

#endif
