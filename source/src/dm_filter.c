/**
 * @file dm_filter.c
 * @brief filter module for datastreams
 *
 * This module can be used to filter a datastream with a specific condition.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/dm_filter.h>

/**
 * @brief incoming data function of the filter in pass mode
 *
 * @param	_filter		pointer to the filter
 * @param	package		the incoming data package
 */
static void dm_filter_new_data_pass(void* const _filter,
		const data_package_t* const package);

/**
 * @brief incoming data function of the filter in block mode
 *
 * @param	_filter		pointer to the filter
 * @param	package		the incoming data package
 */
static void dm_filter_new_data_block(void* const _filter,
		const data_package_t* const package);



void dm_filter_init(dm_filter_t* const filter, int mode,
		datastream_condition_t* const condition) {
	filter->data_out = &data_port_dummy;

	filter->data_in = data_port_dummy;
	filter->data_in.parent = (void*) filter;
	if (mode == FILTER_MODE_PASS) {
		filter->data_in.new_data = dm_filter_new_data_pass;
	} else {
		filter->data_in.new_data = dm_filter_new_data_block;
	}

	filter->condition = condition;
}

data_port_t* dm_filter_get_data_in(dm_filter_t* const filter) {
	return &filter->data_in;
}

void dm_filter_set_data_out(dm_filter_t* const filter,
		const data_port_t* const data_in) {
	filter->data_out = data_in;
}

void dm_filter_set_condition(dm_filter_t* const filter,
		datastream_condition_t* const condition) {
	filter->condition = condition;
}

static void dm_filter_new_data_pass(void* const _filter,
		const data_package_t* const package) {
	dm_filter_t* filter = (dm_filter_t*) _filter;

	datastream_condition_t* const cond = filter->condition;
	if (cond->is_fulfilled(cond, package)) {
		const data_port_t* out = filter->data_out;
		out->new_data(out->parent, package);
	}
}

static void dm_filter_new_data_block(void* const _filter,
		const data_package_t* const package) {
	dm_filter_t* filter = (dm_filter_t*) _filter;

	datastream_condition_t* const cond = filter->condition;
	if (!(cond->is_fulfilled(cond, package))) {
		const data_port_t* out = filter->data_out;
		out->new_data(out->parent, package);
	}
}
