/**
 * @file dm_splitter_control.c
 * @brief splitter module for control-streams
 *
 * This module can be used to split a control-stream and send it to multiple targets.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/dm_splitter_control.h>

/**
 * @brief measure function of the control-stream splitter
 *
 * @param	_splitter		pointer to the splitter
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void dm_splitter_control_new_control_message(void* const _splitter,
		const unsigned int count, const control_parameter_t* parameters);

void dm_splitter_control_init(dm_splitter_control_t* const splitter) {
	int i;
	for (i = 0; i < DM_SPLITTER_CONTROL_CONTROL_OUT_MAX; i++) {
		splitter->control_out[i] = &control_port_dummy;
	}

	splitter->control_in = control_port_dummy;
	splitter->control_in.parent = (void*) splitter;
	splitter->control_in.new_control_message =
			dm_splitter_control_new_control_message;

	splitter->target_count = 0;
}

control_port_t* dm_splitter_control_get_control_in(dm_splitter_control_t* const splitter) {
	return &splitter->control_in;
}

int dm_splitter_control_add_control_out(dm_splitter_control_t* const splitter,
		const control_port_t* const control_in) {
	if (splitter->target_count < DM_SPLITTER_CONTROL_CONTROL_OUT_MAX) {
		splitter->control_out[splitter->target_count++] = control_in;
		return 1;
	} else
		return 0;
}

static void dm_splitter_control_new_control_message(void* const _splitter,
		unsigned int count, const control_parameter_t* parameters) {
	dm_splitter_control_t* splitter = (dm_splitter_control_t*) _splitter;

	int i;
	const control_port_t* out;
	for (i = 0; i < splitter->target_count; i++) {
		out = splitter->control_out[i];
		out->new_control_message(out->parent, count, parameters);
	}
}
