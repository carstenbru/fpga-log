/**
 * @file dm_splitter_control.c
 * @brief splitter module for control-streams
 *
 * This module can be used to split a control-stream and send it to multiple targets.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "dm/dm_splitter_control.h"

/**
 * @brief measure function of the control-stream splitter
 *
 * @param	_splitter	pointer to the splitter
 */
static void dm_splitter_control_measure(void* const _splitter);

void dm_splitter_control_init(dm_splitter_control_t* const splitter) {
	int i;
	for (i = 0; i < DM_SPILTTER_CONTROL_MAX_TARGETS; i++) {
		splitter->control_out[i] = &control_port_dummy;
	}

	splitter->control_in = control_port_dummy;
	splitter->control_in.parent = (void*) splitter;
	splitter->control_in.measure = dm_splitter_control_measure;

	splitter->target_count = 0;
}

int dm_splitter_control_add_control_out(dm_splitter_control_t* const splitter,
		const control_port_t* const control_in) {
	if (splitter->target_count < DM_SPILTTER_CONTROL_MAX_TARGETS) {
		splitter->control_out[splitter->target_count++] = control_in;
		return 1;
	} else
		return 0;
}

static void dm_splitter_control_measure(void* const _splitter) {
	dm_splitter_control_t* splitter = (dm_splitter_control_t*) _splitter;

	int i;
	const control_port_t* out;
	for (i = 0; i < splitter->target_count; i++) {
		out = splitter->control_out[i];
		out->measure(out->parent);
	}
}
