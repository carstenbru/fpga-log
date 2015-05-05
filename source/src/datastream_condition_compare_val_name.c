/**
 * @file datastream_condition_compare_val_name.c
 * @brief datastream compare condition on the value name (equal, not equal)
 *
 * This can be used for example as trigger or filter conditions.
 * The new value is compared with a reference value.
 *
 * The compare string can include '*' for an unknown number of don't care characters
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/datastream_condition_compare_val_name.h>
#include <fpga-log/data_port.h>

static int datastream_condition_compare_val_name_is_fullfilled_equal(
		void* const condition, const data_package_t* const package) {

	const char* s1 =
			((datastream_condition_compare_val_name_t*) condition)->compare_value;
	const char* s2 = package->val_name;
	const char* last_star = 0;
	while (1) {
		if (*s1 != *s2) {
			if (*s1 != '*') {
				if (last_star == 0) {
					return 0;
				} else {
					s1 = last_star;
				}
			} else {
				last_star = s1;
				s2--;  //go one char back in s2 to match current character again
			}
		}
		s1++;
		s2++;
		if (!*s1)  //end of string
			return (!*s2);  //return 1 if string_2 also ends, otherwise 0
		else if (!*s2)
			return 0;
	}
	return 0;
}

static int datastream_condition_compare_val_name_is_fullfilled_not_equal(
		void* const condition, const data_package_t* const package) {

	return !datastream_condition_compare_val_name_is_fullfilled_equal(condition,
			package);
}

void datastream_condition_compare_val_name_init(
		datastream_condition_compare_val_name_t* const cond,
		const datastream_condition_compare_val_name_result_mode mode,
		const char* value) {
	if (mode == RESULT_MODE_EQUAL) {
		cond->super.is_fulfilled =
				datastream_condition_compare_val_name_is_fullfilled_equal;
	} else {
		cond->super.is_fulfilled =
				datastream_condition_compare_val_name_is_fullfilled_not_equal;
	}

	cond->compare_value = value;
}

void datastream_condition_compare_val_name_set_compare_value(
		datastream_condition_compare_val_name_t* const cond, const char* value) {
	cond->compare_value = value;
}

