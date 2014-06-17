/**
 * @file datastream_condition_compare_long_float.c
 * @brief datastream compare conditions for long or simple_float_b10 (equal, smaller, greater,..)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/datastream_condition_compare_long_float.h"
#include "data_port.h"

static int datastream_condition_compare_long_float_is_fullfilled(
		void* const trigger_condition, const data_package_t* const package) {
	datastream_condition_compare_long_float_t* cond =
			(datastream_condition_compare_long_float_t*) trigger_condition;

	switch (package->type) {
	case DATA_TYPE_SIMPLE_FLOAT:
	case DATA_TYPE_LONG:
		return cond->compare_func(*((long*) package->data), cond->compare_value);
	default:
		return 0;  //TODO some error when unsupported data in stream
	}
}

void datastream_condition_compare_long_float_init(
		datastream_condition_compare_long_float_t* const cond,
		int (*compare_func)(const long int val, const long int ref),
		const long int value) {
	cond->super.is_fulfilled =
			datastream_condition_compare_long_float_is_fullfilled;

	cond->compare_func = compare_func;
	cond->compare_value = value;
}

void datastream_condition_compare_long_float_set_compare_value(
		datastream_condition_compare_long_float_t* const cond, const long int value) {
	cond->compare_value = value;
}

int smaller_long(const long int val, const long int ref) {
	return val < ref;
}
int greater_long(const long int val, const long int ref) {
	return val > ref;
}
int equal_long(const long int val, const long int ref) {
	return val == ref;
}
int not_equal_long(const long int val, const long int ref) {
	return val != ref;
}
int smaller_equal_long(const long int val, const long int ref) {
	return val <= ref;
}
int greater_equal_long(const long int val, const long int ref) {
	return val >= ref;
}
