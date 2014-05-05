/**
 * @file control_port.h
 * @brief control_port structure to realize control interface to device drivers
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef CONTROL_PORT_H_
#define CONTROL_PORT_H_

/**
 * @brief struct defining a control destination
 */
typedef struct {
	void* parent; /**< pointer to the parent struct, e.g. a device module */

	/**
	 * @brief pointer to the measure function of the destination
	 *
	 * the device driver should start a measurement when this function is invoked
	 *
	 * @param	parent		the parent stored in this struct
	 */
	void (*measure)(void* const parent);
//TODO multiple function pointers for contol functions
} control_port_t;

/**
 * @brief control_port measure dummy function
 *
 * @param parent	not used
 */
void control_port_measure_dummy(void* const parent);

/**
 * @brief a control_port which can be set when no real control_port is assigned
 */
extern const control_port_t control_port_dummy;

#endif
