/**
 * @file datastream_condition.h
 * @brief datastream condition check
 *
 * This can be used for example as trigger or filter conditions.
 * Use this as super-"class" for more complex conditions.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAM_CONDITION_H_
#define DATASTREAM_CONDITION_H_

#include "data_port.h"

/**
 * @brief struct defining a datastream condition
 *
 * Use this as super-"class" for more complex conditions.
 */
typedef struct {
	/**
	 * @brief checks if the condition is met
	 *
	 * @param trigger_condition	pointer to the condition struct
	 * @param package						pointer to the incoming package
	 * @return									1 if the condition is true, otherwise 0
	 */
	int (*is_fulfilled)(void* const trigger_condition, const data_package_t* const package);
} datastream_condition_t;

/**
 * @brief simple condition which is always true
 */
extern datastream_condition_t datastream_condition_always_true;

#endif
