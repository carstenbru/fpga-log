/**
 * @file dm_core_connector_data_sink.h
 * @brief pseudo-module: this module is used to pass data packages from one SpartanMC system to another, sink side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DM_CORE_CONNECTOR_DATA_SINK_H_
#define DM_CORE_CONNECTOR_DATA_SINK_H_

#include <fpga-log/data_port.h>
#include <peripherals/master_core_connector.h>

/**
 * @brief struct describing a core_connector_data_sink
 */
typedef struct {
	data_port_t data_in; /**< data port, this can be set at a data output to direct the data stream to this module */

	char* last_val_name; /**< pointer to val_name used in last message to save sending it then */

	master_core_connector_regs_t* core_connector; /**< pointer to core connector (master) hardware registers */
} dm_core_connector_data_sink_t;

/**
 * @brief core_connector_data_sink init function
 *
 * initializes the core connector, should be called before using it
 *
 * @param	core_connector_data_sink		pointer to the core_connector_data_sink
 * @param core_connector			pointer to core connector (master) hardware registers
 */
void dm_core_connector_data_sink_init(
		dm_core_connector_data_sink_t* const core_connector_data_sink,
		master_core_connector_regs_t* const core_connector);

/**
 * @brief returns the core_connector_data_sink data input
 *
 * @param splitter		pointer to the core_connector_data_sink
 * @return the data input port
 */
data_port_t* dm_core_connector_data_sink_get_data_in(
		dm_core_connector_data_sink_t* const core_connector_data_sink);

#endif
