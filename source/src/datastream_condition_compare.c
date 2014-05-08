/**
 * @file datastream_condition_compare.c
 * @brief datastream compare conditions (equal, smaller, greater,..)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/datastream_condition_compare.h"

int datastream_condition_compare_is_fullfilled(void* const trigger_condition,
		const data_package_t* const package) {
	datastream_condition_compare_t* cond =
			(datastream_condition_compare_t*) trigger_condition;

	return cond->compare_func(*((int*) package->data), cond->compare_value);
}

void datastream_condition_compare_init(
		datastream_condition_compare_t* const cond,
		int (*compare_func)(const int val, const int ref), const int value) {
	cond->super.is_fullfilled = datastream_condition_compare_is_fullfilled;

	cond->compare_func = compare_func;
	cond->compare_value = value;
}

void datastream_condition_compare_set_compare_value(
		datastream_condition_compare_t* const cond, const int value) {
	cond->compare_value = value;
}

int smaller(const int val, const int ref) {
	return val < ref;
}
int greater(const int val, const int ref) {
	return val > ref;
}
int equal(const int val, const int ref) {
	return val == ref;
}
int not_equal(const int val, const int ref) {
	return val != ref;
}
int smaller_equal(const int val, const int ref) {
	return val <= ref;
}
int greater_equal(const int val, const int ref) {
	return val >= ref;
}
