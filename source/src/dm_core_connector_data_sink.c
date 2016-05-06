/**
 * @file dm_core_connector_data_sink.c
 * @brief pseudo-module: this module is used to pass data packages from one SpartanMC system to another, sink side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/dm_core_connector_data_sink.h>

/**
 * @brief new data function of the core_connector_data_sink
 *
 * @param	_core_connector_data_sink		pointer to the core_connector_data_sink
 * @param package			incoming data package
 */
static void dm_core_connector_data_sink_new_data(
		void* const _core_connector_data_sink, const data_package_t* const package);

void dm_core_connector_data_sink_init(
		dm_core_connector_data_sink_t* const core_connector_data_sink,
		master_core_connector_regs_t* const core_connector) {
	core_connector_data_sink->data_in = data_port_dummy;
	core_connector_data_sink->data_in.parent = (void*) core_connector_data_sink;
	core_connector_data_sink->data_in.new_data =
			dm_core_connector_data_sink_new_data;

	core_connector_data_sink->core_connector = core_connector;
	core_connector_data_sink->last_val_name = 0;
}

data_port_t* dm_core_connector_data_sink_get_data_in(
		dm_core_connector_data_sink_t* const core_connector_data_sink) {
	return &core_connector_data_sink->data_in;
}

static void dm_core_connector_data_sink_new_data(
		void* const _core_connector_data_sink, const data_package_t* const package) {
	dm_core_connector_data_sink_t* core_connector_data_sink =
			(dm_core_connector_data_sink_t*) _core_connector_data_sink;
	master_core_connector_regs_t* core_connector =
			core_connector_data_sink->core_connector;

	int data_size;
	switch (package->type) {
	case (DATA_TYPE_INT):
		data_size = 1;
		break;
	case (DATA_TYPE_LONG):
		data_size = 2;
		break;
	case (DATA_TYPE_SIMPLE_FLOAT):
		data_size = 3;
		break;
	case (DATA_TYPE_CHAR):
		data_size = 1;
		break;
	default:
		//TODO error message, unsupported data type!!!
		break;
	}

	//TODO add val_name size
	core_connector->msg_size = 8 + data_size;  //size+source_id+val_name(just an ID of that)+type +timestamp(4) +data(!)
	if (core_connector->status != MASTER_FIFO_FULL) {  //check if we have enough space in the fifo
		core_connector->data_out = data_size;
		core_connector->data_out = package->source_id;

		if (core_connector_data_sink->last_val_name != package->val_name) {
			core_connector_data_sink->last_val_name = package->val_name;
			const char* c = package->val_name;
			while (*c != 0) {
				core_connector->data_out = (unsigned int) *c;
				c++;
			}
		}  //if val_name is last as in last package we do not need to send it
		core_connector->data_out = 0;

		core_connector->data_out = package->type;

		core_connector->data_out = package->timestamp->lpt_union.lpt_struct.lpt_low;
		core_connector->data_out =
				package->timestamp->lpt_union.lpt_struct.lpt_high;
		core_connector->data_out = package->timestamp->hpt_union.hpt_struct.hpt_low;
		core_connector->data_out =
				package->timestamp->hpt_union.hpt_struct.hpt_high;

		const unsigned int* data = package->data;
		while (data_size--) {
			core_connector->data_out = *data++;
		}
	} else {
		//TODO some error handling code here! FiFo overflow!
	}
}
