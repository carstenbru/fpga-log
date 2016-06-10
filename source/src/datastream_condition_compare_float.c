/**
 * @file datastream_condition_compare_float.c
 * @brief datastream compare conditions for simple_float_b10 (equal, smaller, greater,..)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/datastream_condition_compare_float.h>
#include <fpga-log/data_port.h>

static int datastream_condition_compare_float_is_fullfilled(
		void* const trigger_condition, const data_package_t* const package) {
	datastream_condition_compare_float_t* cond =
			(datastream_condition_compare_float_t*) trigger_condition;

	if (package->type == DATA_TYPE_SIMPLE_FLOAT) {
		return cond->compare_func(*((simple_float_b10_t*) package->data),
				cond->compare_value);
	} else {
		return 0;  //TODO some error when unsupported data in stream
	}
}

void datastream_condition_compare_float_init(
		datastream_condition_compare_float_t* const cond,
		int (*compare_func)(const simple_float_b10_t val,
				const simple_float_b10_t ref), const simple_float_b10_t value) {
	cond->super.is_fulfilled = datastream_condition_compare_float_is_fullfilled;

	cond->compare_func = compare_func;
	cond->compare_value = value;
}

void datastream_condition_compare_float_set_compare_value(
		datastream_condition_compare_float_t* const cond,
		const simple_float_b10_t value) {
	cond->compare_value = value;
}

int smaller_float(const simple_float_b10_t val, const simple_float_b10_t ref) {
	if ((val.coefficient < 0) ^ (ref.coefficient < 0)) {  //different sign
		return (val.coefficient < 0);
	} else {  //same sign
		if (val.exponent < ref.exponent)
			return 1;
		else if (val.exponent == ref.exponent)
			return (val.coefficient < ref.coefficient);
		else
			return 0;
	}
}

int greater_float(const simple_float_b10_t val, const simple_float_b10_t ref) {
	if ((val.coefficient < 0) ^ (ref.coefficient < 0)) {  //different sign
		return (val.coefficient > 0);
	} else {  //same sign
		if (val.exponent > ref.exponent)
			return 1;
		else if (val.exponent == ref.exponent)
			return (val.coefficient > ref.coefficient);
		else
			return 0;
	}
}

int equal_float(const simple_float_b10_t val, const simple_float_b10_t ref) {
	return ((val.exponent == ref.exponent) && (val.coefficient == ref.coefficient));
}

int not_equal_float(const simple_float_b10_t val, const simple_float_b10_t ref) {
	return ((val.exponent != ref.exponent) || (val.coefficient != ref.coefficient));
}

int smaller_equal_float(const simple_float_b10_t val,
		const simple_float_b10_t ref) {
	return (smaller_float(val, ref) || equal_float(val, ref));
}

int greater_equal_float(const simple_float_b10_t val,
		const simple_float_b10_t ref) {
	return (greater_float(val, ref) || equal_float(val, ref));
}
