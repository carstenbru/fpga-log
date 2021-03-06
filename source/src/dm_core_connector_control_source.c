/**
 * @file dm_core_connector_control_source.c
 * @brief pseudo-module: this module is used to pass control messages from one SpartanMC system to another, source side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/dm_core_connector_control_source.h>

/**
 * @brief core_connector_control_source update function
 *
 * @param	_core_connector_control_source	pointer to the core_connector_control_source
 */
static void dm_core_connector_control_source_update(
		void* const _core_connector_control_source);

void dm_core_connector_control_source_init(
		dm_core_connector_control_source_t* const core_connector_control_source,
		slave_core_connector_regs_t* const core_connector) {
	datastream_object_init(&core_connector_control_source->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	core_connector_control_source->super.update =
			dm_core_connector_control_source_update;

	core_connector_control_source->control_out = &control_port_dummy;

	core_connector_control_source->core_connector = core_connector;
	core_connector->msg_size = 1;  //set to 1 as we don't know the real message size...only used to check for new messages anyway
}

void dm_core_connector_control_source_set_control_out(
		dm_core_connector_control_source_t* const core_connector_control_source,
		const control_port_t* const control_in) {
	core_connector_control_source->control_out = control_in;
}

static void dm_core_connector_control_source_update(
		void* const _core_connector_control_source) {
	dm_core_connector_control_source_t* core_connector_control_source =
			(dm_core_connector_control_source_t*) _core_connector_control_source;
	slave_core_connector_regs_t* core_connector =
			core_connector_control_source->core_connector;

	while (core_connector->status != SLAVE_FIFO_EMPTY) {
		int length = core_connector->data_in;
		int count = length;
		control_parameter_t* buf = core_connector_control_source->buf;
		while (count > CORE_CONNECTOR_CONTROL_SOURCE_MAX_PARAMETERS) {  //throw away first parameters if too many to handle in message
			//TODO error message!
			buf->type = core_connector->data_in;
			buf->value = core_connector->data_in;
			count--;
		}
		while (count--) { //read parameters to buffer
			buf->type = core_connector->data_in;
			buf->value = core_connector->data_in;
			buf++;
		}
		core_connector_control_source->control_out->new_control_message(
				core_connector_control_source->control_out->parent, length,
				core_connector_control_source->buf); //send out received message at our control port
	}
}
