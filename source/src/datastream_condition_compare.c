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
#include "data_port.h"

static int datastream_condition_compare_is_fullfilled_value(
		void* const trigger_condition, const data_package_t* const package) {
	datastream_condition_compare_t* cond =
			(datastream_condition_compare_t*) trigger_condition;

	int val = 0;
	switch (package->type) {
	case DATA_TYPE_INT:
		val = *((int*) package->data);
		break;
	case DATA_TYPE_BYTE:
		val = *((unsigned char*) package->data);
		break;
	default:
		break;  //TODO some error when unsupported data in stream
	}

	return cond->compare_func(val, cond->compare_value);
}

static int datastream_condition_compare_is_fullfilled_source_id(
		void* const trigger_condition, const data_package_t* const package) {
	datastream_condition_compare_t* cond =
			(datastream_condition_compare_t*) trigger_condition;

	return cond->compare_func(package->source_id, cond->compare_value);
}

static int datastream_condition_compare_is_fullfilled_value_id(
		void* const trigger_condition, const data_package_t* const package) {
	datastream_condition_compare_t* cond =
			(datastream_condition_compare_t*) trigger_condition;

	return cond->compare_func(package->val_name, cond->compare_value);
}

void datastream_condition_compare_init(
		datastream_condition_compare_t* const cond,
		int (*compare_func)(const int val, const int ref), const int mode,
		const int value) {
	if (mode == COMPARE_MODE_SOURCE_ID) {
		cond->super.is_fulfilled =
				datastream_condition_compare_is_fullfilled_source_id;
	} else if (mode == COMPARE_MODE_VALUE) {
		cond->super.is_fulfilled = datastream_condition_compare_is_fullfilled_value;
	} else {
		cond->super.is_fulfilled =
				datastream_condition_compare_is_fullfilled_value_id;
	}

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
