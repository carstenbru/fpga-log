/**
 * @file dm_core_connector_control_sink.h
 * @brief pseudo-module: this module is used to pass control messages from one SpartanMC system to another, sink side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_CORE_CONNECTOR_CONTROL_SINK_H_
#define DM_CORE_CONNECTOR_CONTROL_SINK_H_

#include <fpga-log/control_port.h>
#include <peripherals/master_core_connector.h>

/**
 * @brief struct describing a core_connector_control_sink
 */
typedef struct {
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this module */

	master_core_connector_regs_t* core_connector; /**< pointer to core connector (master) hardware registers */
} dm_core_connector_control_sink_t;

/**
 * @brief core_connector_control_sink init function
 *
 * initializes the core connector, should be called before using it
 *
 * @param	core_connector_control_sink		pointer to the core_connector_control_sink
 * @param core_connector			pointer to core connector (master) hardware registers
 */
void dm_core_connector_control_sink_init(
		dm_core_connector_control_sink_t* const core_connector_control_sink,
		master_core_connector_regs_t* const core_connector);

/**
 * @brief returns the core_connector_control_sink control input
 *
 * @param splitter		pointer to the core_connector_control_sink
 * @return the control input port
 */
control_port_t* dm_core_connector_control_sink_get_control_in(
		dm_core_connector_control_sink_t* const core_connector_control_sink);

#endif
