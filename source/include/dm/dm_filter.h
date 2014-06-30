/**
 * @file dm_filter.h
 * @brief filter module for datastreams
 *
 * This module can be used to filter a datastream with a specific condition.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_FILTER_H_
#define DM_FILTER_H_

#include "data_port.h"
#include "dm/datastream_condition.h"

/**
 * @brief enumeration of the different filter modes
 */
typedef enum {
	FILTER_MODE_PASS, /**< the filter lets all packages pass which fulfill the condition */
	FILTER_MODE_BLOCK /**< the filter blocks all packages which fulfill the condition */
} dm_filter_mode;

/**
 * @brief struct describing a datastream filter
 */
typedef struct {
	data_port_t data_in; /**< data port, this can be set at a data output to direct the data stream to this trigger */
	const data_port_t* data_out; /**< data output port */

	datastream_condition_t* condition; /**< filter condition defining which packages are blocked or pass */
} dm_filter_t;

/**
 * @brief datastream filter init function
 *
 * Initializes the filter, should be called before using the filter.
 *
 * @param	filter		pointer to the datastream filter
 * @param mode			operating mode of the filter, should be a value of enumeration @ref dm_filter_mode
 * @param condition	the filter condition
 */
void dm_filter_init(dm_filter_t* const filter, int mode,
		datastream_condition_t* const condition);

/**
 * @brief returns the datastream filter data input
 *
 * @param filter		 pointer to the datastream filter
 * @return the data input port
 */
data_port_t* dm_filter_get_data_in(dm_filter_t* const filter);

/**
 * @brief connects the data output port of a filter to a given destination
 *
 * @param	filter			pointer to the filter
 * @param	data_in			the new data destination
 */
void dm_filter_set_data_out(dm_filter_t* const filter,
		const data_port_t* const data_in);

/**
 * @brief sets a new filter condition
 *
 * @param filter		pointer to the filter
 * @param condition	the new filter condition
 */
void dm_filter_set_condition(dm_filter_t* const filter,
		datastream_condition_t* const condition);

#endif
