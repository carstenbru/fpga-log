/**
 * @file datastream_condition_compare_int.c
 * @brief datastream compare conditions for integer and char (equal, smaller, greater,..)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/datastream_condition_compare_int.h>
#include <fpga-log/data_port.h>

static int datastream_condition_compare_int_is_fullfilled_value(
		void* const trigger_condition, const data_package_t* const package) {
	datastream_condition_compare_int_t* cond =
			(datastream_condition_compare_int_t*) trigger_condition;

	switch (package->type) {
	case DATA_TYPE_INT:
		return cond->compare_func(*((int*) package->data), cond->compare_value);
	case DATA_TYPE_BYTE:
		return cond->compare_func(*((unsigned char*) package->data),
				cond->compare_value);
	default:
		return 0;  //TODO some error when unsupported data in stream
	}
}

static int datastream_condition_compare_int_is_fullfilled_source_id(
		void* const trigger_condition, const data_package_t* const package) {
	datastream_condition_compare_int_t* cond =
			(datastream_condition_compare_int_t*) trigger_condition;

	return cond->compare_func(package->source_id, cond->compare_value);
}

void datastream_condition_compare_int_init(
		datastream_condition_compare_int_t* const cond,
		int (*compare_func)(const int val, const int ref),
		const datastream_condition_compare_int_mode mode, const int value) {
	if (mode == COMPARE_MODE_SOURCE_ID) {
		cond->super.is_fulfilled =
				datastream_condition_compare_int_is_fullfilled_source_id;
	} else {
		cond->super.is_fulfilled =
				datastream_condition_compare_int_is_fullfilled_value;
	}

	cond->compare_func = compare_func;
	cond->compare_value = value;
}

void datastream_condition_compare_int_set_compare_value(
		datastream_condition_compare_int_t* const cond, const int value) {
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
