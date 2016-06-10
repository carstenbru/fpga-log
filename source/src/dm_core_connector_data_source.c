/**
 * @file dm_core_connector_data_source.c
 * @brief pseudo-module: this module is used to pass data packages from one SpartanMC system to another, source side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/dm_core_connector_data_source.h>

/**
 * @brief core_connector_data_source update function
 *
 * @param	_core_connector_data_source	pointer to the core_connector_data_source
 */
static void dm_core_connector_data_source_update(
		void* const _core_connector_data_source);

void dm_core_connector_data_source_init(
		dm_core_connector_data_source_t* const core_connector_data_source,
		slave_core_connector_regs_t* const core_connector) {
	datastream_object_init(&core_connector_data_source->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	core_connector_data_source->super.update =
			dm_core_connector_data_source_update;

	core_connector_data_source->data_out = &data_port_dummy;

	core_connector_data_source->core_connector = core_connector;
	core_connector->msg_size = 1;  //set to 1 as we don't know the real message size...only used to check for new messages anyway

	core_connector_data_source->buffer_package.timestamp =
			&core_connector_data_source->timestamp_buf;
	core_connector_data_source->buffer_package.data =
			&core_connector_data_source->data_buf;
	core_connector_data_source->buffer_package.val_name =
			&core_connector_data_source->val_name_buf;
}

void dm_core_connector_data_source_set_data_out(
		dm_core_connector_data_source_t* const core_connector_data_source,
		const data_port_t* const data_in) {
	core_connector_data_source->data_out = data_in;
}

static void dm_core_connector_data_source_update(
		void* const _core_connector_data_source) {
	dm_core_connector_data_source_t* core_connector_data_source =
			(dm_core_connector_data_source_t*) _core_connector_data_source;
	slave_core_connector_regs_t* core_connector =
			core_connector_data_source->core_connector;

	while (core_connector->status != SLAVE_FIFO_EMPTY) {
		int data_size = core_connector->data_in;
		data_package_t* buf = &core_connector_data_source->buffer_package;
		buf->source_id = core_connector->data_in;

		char c = core_connector->data_in;
		if (c != 0) {  //immediate 0 means use last val_name
			char* val_name_buf = core_connector_data_source->val_name_buf;
			*val_name_buf++ = c;
			do {
				c = core_connector->data_in;
				*val_name_buf++ = c;
			} while (c != 0);
		}

		buf->type = core_connector->data_in;

		timestamp_t* tbuf = &core_connector_data_source->timestamp_buf;
		tbuf->lpt_union.lpt_struct.lpt_low = core_connector->data_in;
		tbuf->lpt_union.lpt_struct.lpt_high = core_connector->data_in;
		tbuf->hpt_union.hpt_struct.hpt_low = core_connector->data_in;
		tbuf->hpt_union.hpt_struct.hpt_high = core_connector->data_in;

		unsigned int* data = core_connector_data_source->data_buf;
		while (data_size--) {
			*data++ = core_connector->data_in;
		}

		core_connector_data_source->data_out->new_data(
				core_connector_data_source->data_out->parent, buf);
	}
}
