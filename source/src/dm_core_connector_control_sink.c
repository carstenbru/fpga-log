/**
 * @file dm_core_connector_control_sink.c
 * @brief pseudo-module: this module is used to pass control messages from one SpartanMC system to another, sink side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/dm/dm_core_connector_control_sink.h>

/**
 * @brief new control message function of the core_connector_control_sink
 *
 * @param	_core_connector_control_sink		pointer to the core_connector_control_sink
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void dm_core_connector_control_sink_new_control_message(
		void* const _core_connector_control_sink, unsigned int count,
		const control_parameter_t* parameters);

void dm_core_connector_control_sink_init(
		dm_core_connector_control_sink_t* const core_connector_control_sink,
		master_core_connector_regs_t* const core_connector) {
	core_connector_control_sink->control_in = control_port_dummy;
	core_connector_control_sink->control_in.parent =
			(void*) core_connector_control_sink;
	core_connector_control_sink->control_in.new_control_message =
			dm_core_connector_control_sink_new_control_message;

	core_connector_control_sink->core_connector = core_connector;
}

control_port_t* dm_core_connector_control_sink_get_control_in(
		dm_core_connector_control_sink_t* const core_connector_control_sink) {
	return &core_connector_control_sink->control_in;
}
static void dm_core_connector_control_sink_new_control_message(
		void* const _core_connector_control_sink, unsigned int count,
		const control_parameter_t* parameters) {
	master_core_connector_regs_t* core_connector =
			((dm_core_connector_control_sink_t*) _core_connector_control_sink)->core_connector;

	core_connector->msg_size = count * sizeof(control_parameter_t) + 1;  //we have count parameter plus one integer message size
	if (core_connector->status != MASTER_FIFO_FULL) {  //check if we have enough space in the fifo
		core_connector->data_out = count;
		while (count--) {  //write all parameters into fifo
			core_connector->data_out = parameters->type;
			core_connector->data_out = parameters->value;
			parameters++;
		}
	} else {
		//TODO some error handling code here! FiFo overflow!
	}
}
