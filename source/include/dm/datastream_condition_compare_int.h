/**
 * @file datastream_condition_compare_int.h
 * @brief datastream compare conditions for integer and char (equal, smaller, greater,..)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 * The condition can also be set on the source id instead the measured value.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAM_CONDITION_COMPARE_INT_H_
#define DATASTREAM_CONDITION_COMPARE_INT_H_

#include "dm/datastream_condition.h"

/**
 * @brief enumeration of the different compare modes
 */
typedef enum {
	COMPARE_MODE_VALUE, /**< the measured value is compared with the reference value */
	COMPARE_MODE_SOURCE_ID /**< the source id is compared with the reference value */
} datastream_condition_compare_int_mode;

/**
 * @brief struct defining a compare condition
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
	int (*compare_func)(const int val, const int ref);
	int compare_value; /**< value which is used for the comparison */
} datastream_condition_compare_int_t;

/**
 * @brief initializes the compare condition
 *
 * @param cond			pointer to the compare condition to initialize
 * @param comp_func	compare function which compares new value with the reference value
 * @param mode			mode of the compare condition, should be a value of enumeration @ref datastream_condition_compare_mode
 * @param value			new compare value
 */
void datastream_condition_compare_int_init(
		datastream_condition_compare_int_t* const cond,
		int (*compare_func)(const int val, const int ref),
		const datastream_condition_compare_int_mode mode, const int value);

/**
 * @brief sets a new compare value of the condition
 *
 * @param cond	the condition to modify
 * @param value	the new value
 */
void datastream_condition_compare_int_set_compare_value(
		datastream_condition_compare_int_t* const cond, const int value);

/*
 * different compare functions
 */
int smaller(const int val, const int ref);
int greater(const int val, const int ref);
int equal(const int val, const int ref);
int not_equal(const int val, const int ref);
int smaller_equal(const int val, const int ref);
int greater_equal(const int val, const int ref);

#endif
