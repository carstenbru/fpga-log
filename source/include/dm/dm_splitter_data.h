/**
 * @file dm_splitter_data.h
 * @brief splitter module for datastreams
 *
 * This module can be used to split a data-stream and send it to multiple targets.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_SPLITTER_DATA_H_
#define DM_SPLITTER_DATA_H_

/**
 * @brief number of maximal targets for a datastream splitter
 *
 * Since we have no heap (malloc) on SpartanMC we need to allocate a fixed amount of memory with the struct.
 */
#define DM_SPLITTER_DATA_DATA_OUT_MAX 8

#include "data_port.h"

/**
 * @brief struct describing a datastream splitter
 */
typedef struct {
	data_port_t data_in; /**< data port, this can be set at a data output to direct the data stream to this splitter */
	const data_port_t* data_out[DM_SPLITTER_DATA_DATA_OUT_MAX]; /**< data output ports */

	int target_count; /**< currently assigned targets */
} dm_splitter_data_t;

/**
 * @brief datastream splitter init function
 *
 * initializes the splitter, should be called before using the splitter
 *
 * @param	splitter		pointer to the datastream splitter
 */
void dm_splitter_data_init(dm_splitter_data_t* const splitter);

/**
 * @brief returns the datastream splitter data input
 *
 * @param splitter		 pointer to the datastream splitter
 * @return the data input port
 */
data_port_t dm_splitter_data_get_data_in(dm_splitter_data_t* const splitter);

/**
 * @brief adds a new data destination to the splitter
 *
 * @param	splitter		pointer to the datastream splitter
 * @param	data_in			the new data destination
 *
 * @return 1 on success, otherwise 0 (e.g. max destinations already reached)
 */
int dm_splitter_data_add_data_out(dm_splitter_data_t* const splitter,
		const data_port_t* const data_in);


#endif
