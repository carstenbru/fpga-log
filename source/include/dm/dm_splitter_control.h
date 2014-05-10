/**
 * @file dm_splitter_control.h
 * @brief splitter module for control-streams
 *
 * This module can be used to split a control-stream and send it to multiple targets.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DM_SPLITTER_CONTROL_H_
#define DM_SPLITTER_CONTROL_H_

/**
 * @brief number of maximal targets for a control-stream splitter
 *
 * Since we have no heap (malloc) on SpartanMC we need to allocate a fixed amount of memory with the struct.
 */
#define DM_SPILTTER_CONTROL_MAX_TARGETS 8

#include "control_port.h"

/**
 * @brief struct describing a control-stream splitter
 */
typedef struct {
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this splitter */
	const control_port_t* control_out[DM_SPILTTER_CONTROL_MAX_TARGETS]; /**< control output ports */

	int target_count; /**< currently assigned targets */
} dm_splitter_control_t;

/**
 * @brief control-stream splitter init function
 *
 * initializes the splitter, should be called before using the splitter
 *
 * @param	splitter		pointer to the control-stream splitter
 */
void dm_splitter_control_init(dm_splitter_control_t* const splitter);

/**
 * @brief adds a new control destination to the splitter
 *
 * @param	splitter		pointer to the control-stream splitter
 * @param	control_in	the new control destination
 *
 * @return 1 on success, otherwise 0 (e.g. max destinations already reached)
 */
int dm_splitter_control_add_control_out(dm_splitter_control_t* const splitter,
		const control_port_t* const control_in);

#endif
