/**
 * @file dm_core_connector_control_source.h
 * @brief pseudo-module: this module is used to pass control messages from one SpartanMC system to another, source side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_CORE_CONNECTOR_CONTROL_SOURCE_H_
#define DM_CORE_CONNECTOR_CONTROL_SOURCE_H_

#include <fpga-log/datastream_object.h>
#include <fpga-log/control_port.h>
#include <peripherals/slave_core_connector.h>

#define CORE_CONNECTOR_CONTROL_SOURCE_MAX_PARAMETERS 8

/**
 * @brief struct describing a core_connector_control_source
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/

	const control_port_t* control_out; /**< control output port */

	control_parameter_t buf[CORE_CONNECTOR_CONTROL_SOURCE_MAX_PARAMETERS]; /**< buffer for reading control messages */

	slave_core_connector_regs_t* core_connector; /**< pointer to core connector (slave) hardware registers */
} dm_core_connector_control_source_t;

/**
 * @brief core_connector_control_source init function
 *
 * initializes the core connector, should be called before using it
 *
 * @param	core_connector_control_source		pointer to the core_connector_control_source
 * @param core_connector			pointer to core connector (slave) hardware registers
 */
void dm_core_connector_control_source_init(
		dm_core_connector_control_source_t* const core_connector_control_source,
		slave_core_connector_regs_t* const core_connector);

/**
 * @brief connects the control output port of a core_connector_control_source to a given destination
 *
 * @param	core_connector_control_source	pointer to the core_connector_control_source
 * @param	control_in	the new control destination
 */
void dm_core_connector_control_source_set_control_out(
		dm_core_connector_control_source_t* const core_connector_control_source,
		const control_port_t* const control_in);

#endif
