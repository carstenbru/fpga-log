/**
 * @file dm_core_connector_data_source.h
 * @brief pseudo-module: this module is used to pass data packages from one SpartanMC system to another, source side
 *
 * This module is transparent to the user, usually instantiated automatically by the config-tool.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_CORE_CONNECTOR_DATA_SOURCE_H_
#define DM_CORE_CONNECTOR_DATA_SOURCE_H_

#include <fpga-log/datastream_object.h>
#include <fpga-log/data_port.h>
#include <peripherals/slave_core_connector.h>

#define DM_CORE_CONNECTOR_DATA_SOURCE_MAX_DATA_LENGTH 3
#define DM_CORE_CONNECTOR_DATA_SOURCE_MAX_VAL_NAME_LENGTH 32

/**
 * @brief struct describing a core_connector_data_source
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/

	const data_port_t* data_out; /**< data output port */

	unsigned int data_buf[DM_CORE_CONNECTOR_DATA_SOURCE_MAX_DATA_LENGTH]; /**< buffer for received data */
	char val_name_buf[DM_CORE_CONNECTOR_DATA_SOURCE_MAX_VAL_NAME_LENGTH]; /**< buffer for value name */
	timestamp_t timestamp_buf; /**< timestamp_buffer */
	data_package_t buffer_package; /**< buffer to store received package */

	slave_core_connector_regs_t* core_connector; /**< pointer to core connector (slave) hardware registers */
} dm_core_connector_data_source_t;

/**
 * @brief core_connector_data_source init function
 *
 * initializes the core connector, should be called before using it
 *
 * @param	core_connector_data_source		pointer to the core_connector_data_source
 * @param core_connector			pointer to core connector (slave) hardware registers
 */
void dm_core_connector_data_source_init(
		dm_core_connector_data_source_t* const core_connector_data_source,
		slave_core_connector_regs_t* const core_connector);

/**
 * @brief connects the data output port of a core_connector_data_source to a given destination
 *
 * @param	core_connector_data_source	pointer to the core_connector_data_source
 * @param	data_in	the new data destination
 */
void dm_core_connector_data_source_set_data_out(
		dm_core_connector_data_source_t* const core_connector_data_source,
		const data_port_t* const data_in);

#endif
