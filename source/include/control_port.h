/**
 * @file control_port.h
 * @brief control_port structure to realize control interface to device drivers
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef CONTROL_PORT_H_
#define CONTROL_PORT_H_

/**
 * @brief struct holding incoming data
 */
typedef struct {
	int type; /**< the type of the control parameter */
	int value; /**< parameter value */
} control_parameter_t;

/**
 * @brief struct defining a control destination
 */
typedef struct {
	void* parent; /**< pointer to the parent struct, e.g. a device module */

	/**
	 * @brief pointer to the control message function of the destination
	 *
	 * @param count				amount of passed parameter structs
	 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
	 */
	void (*new_control_message)(void* const parent, unsigned int count,
			const control_parameter_t* parameters);
} control_port_t;

/**
 * @brief a control_port dummy
 *
 * can be set when no real control_port is assigned
 * or before real values are assigned so that no function pointer is null
 */
extern const control_port_t control_port_dummy;

#endif
