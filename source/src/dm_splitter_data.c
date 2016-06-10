/**
 * @file dm_splitter_data.c
 * @brief splitter module for datastreams
 *
 * This module can be used to split a data-stream and send it to multiple targets.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/dm_splitter_data.h>

/**
 * @brief incoming data function of the datastream splitter
 *
 * @param	_splitter	pointer to the splitter
 * @param	package		the incoming data package
 */
static void dm_splitter_data_new_data(void* const _splitter,
		const data_package_t* const package);

void dm_splitter_data_init(dm_splitter_data_t* const splitter) {
	int i;
	for (i = 0; i < DM_SPLITTER_DATA_DATA_OUT_MAX; i++) {
		splitter->data_out[i] = &data_port_dummy;
	}

	splitter->data_in = data_port_dummy;
	splitter->data_in.parent = (void*) splitter;
	splitter->data_in.new_data = dm_splitter_data_new_data;

	splitter->target_count = 0;
}

data_port_t* dm_splitter_data_get_data_in(dm_splitter_data_t* const splitter) {
	return &splitter->data_in;
}

int dm_splitter_data_add_data_out(dm_splitter_data_t* const splitter,
		const data_port_t* const data_in) {
	if (splitter->target_count < DM_SPLITTER_DATA_DATA_OUT_MAX) {
		splitter->data_out[splitter->target_count++] = data_in;
		return 1;
	} else
		return 0;
}

static void dm_splitter_data_new_data(void* const _splitter,
		const data_package_t* const package) {
	dm_splitter_data_t* splitter = (dm_splitter_data_t*) _splitter;

	int i;
	const data_port_t* out;
	for (i = 0; i < splitter->target_count; i++) {
		out = splitter->data_out[i];
		out->new_data(out->parent, package);
	}
}
