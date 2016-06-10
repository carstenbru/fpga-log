/**
 * @file datastream_condition_compare_val_name.h
 * @brief datastream compare condition on the value name (equal, not equal)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DATASTREAM_CONDITION_COMPARE_VAL_NAME_H_
#define DATASTREAM_CONDITION_COMPARE_VAL_NAME_H_

#include <fpga-log/dm/datastream_condition.h>

/**
 * @brief enumeration of the different compare modes
 */
typedef enum {
	RESULT_MODE_EQUAL, /**< the condition is true if the value name is equal to the reference value */
	RESULT_MODE_NOT_EQUAL /**< the condition is true if the value name is not equal to the reference value */
} datastream_condition_compare_val_name_result_mode;

/**
 * @brief struct defining a compare condition
 */
typedef struct {
	datastream_condition_t super; /**< super-"class": datastream_condition_t */

	const char* compare_value; /**< value which is used for the comparison */
} datastream_condition_compare_val_name_t;

/**
 * @brief initializes the compare condition
 *
 * @param cond			pointer to the compare condition to initialize
 * @param mode			mode of the compare condition, should be a value of enumeration @ref datastream_condition_compare_val_name_result_mode
 * @param value			new compare value
 */
void datastream_condition_compare_val_name_init(
		datastream_condition_compare_val_name_t* const cond,
		const datastream_condition_compare_val_name_result_mode mode,
		const char* value);

/**
 * @brief sets a new compare value of the condition
 *
 * @param cond	the condition to modify
 * @param value	the new value
 */
void datastream_condition_compare_val_name_set_compare_value(
		datastream_condition_compare_val_name_t* const cond, const char* value);

#endif
