/**
 * @file datastream_condition.c
 * @brief datastream condition check
 *
 * This can be used for example as trigger or filter conditions.
 * Use this as super-"class" for more complex conditions.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/datastream_condition.h"

int cond_always_true(void* const trigger_condition, const data_package_t* const package) {
	return 1;
}

datastream_condition_t datastream_condition_always_true = {cond_always_true};
