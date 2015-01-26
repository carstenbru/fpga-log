/**
 * @file datastream_condition_logic.h
 * @brief logic connection between two datastream conditions
 *
 * This can be used to create more complex conditions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAM_CONDITION_LOGIC_H_
#define DATASTREAM_CONDITION_LOGIC_H_

#include <fpga-log/dm/datastream_condition.h>

/**
 * @brief enumeration of the different logic operators
 */
typedef enum {
	OPERATOR_AND, /**< the condition is true if both conditions are true */
	OPERATOR_OR /**< the condition is true if one of the conditions is true */
} datastream_condition_logic_operator;

/**
 * @brief struct defining a logic conneciton condition
 */
typedef struct {
	datastream_condition_t super; /**< super-"class": datastream_condition_t */

	datastream_condition_t* first; /**< pointer to the first condition */
	datastream_condition_t* second; /**< pointer to the second condition */
} datastream_condition_logic_t;

void datastream_condition_logic_init(datastream_condition_logic_t* const cond,
		const datastream_condition_t* first, datastream_condition_t* second,
		datastream_condition_logic_operator operator);

#endif
