/**
 * @file datastream_condition_logic.c
 * @brief logic connection between two datastream conditions
 *
 * This can be used to create more complex conditions
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/datastream_condition_logic.h>

static int datastream_condition_logic_is_fullfilled_AND(void* const condition,
		const data_package_t* const package) {
	datastream_condition_logic_t* cond = (datastream_condition_logic_t*) condition;

	if (!cond->first->is_fulfilled(cond->first, package)) {
		return 0;  //short-circuit evaluation
	} else {
		return cond->second->is_fulfilled(cond->second, package);
	}
}

static int datastream_condition_logic_is_fullfilled_OR(void* const condition,
		const data_package_t* const package) {
	datastream_condition_logic_t* cond = (datastream_condition_logic_t*) condition;

	if (cond->first->is_fulfilled(cond->first, package)) {
		return 1;  //short-circuit evaluation
	} else {
		return cond->second->is_fulfilled(cond->second, package);
	}
}

void datastream_condition_logic_init(datastream_condition_logic_t* const cond,
		datastream_condition_t* first, datastream_condition_t* second,
		datastream_condition_logic_operator operator) {
	switch (operator) {
	case OPERATOR_AND:
		cond->super.is_fulfilled = datastream_condition_logic_is_fullfilled_AND;
		break;
	case OPERATOR_OR:
		cond->super.is_fulfilled = datastream_condition_logic_is_fullfilled_OR;
		break;
	}

	cond->first = first;
	cond->second = second;
}
