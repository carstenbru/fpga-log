/**
 * @file data_port.h
 * @brief data_port and data_package structures to realize datastreams from devices to sinks
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATA_PORT_H_
#define DATA_PORT_H_

/**
 * @brief enumeration of the different data types which can be in a data package
 */
typedef enum {
	DATA_TYPE_INT, /**< single integer value */
	DATA_TYPE_BYTE /**< single byte value */
} data_type;

/**
 * @brief struct holding incoming data
 */
typedef struct {
	const int source_id; 		/**< the id of the data source, e.g. to show in logs */
	const int type;					/**< the type of the data value, should be a value of enumeration @ref data_type */
	const void* const data; /**< pointer to the actual data */
} data_package_t;

/**
 * @brief struct defining a data destination
 */
typedef struct {
	void* parent; /**< pointer to the parent struct, e.g. a datasink module */

	/**
	 * @brief pointer to the new data function of the destination
	 *
	 * @param	parent		the parent stored in this struct
	 * @param	package		the incoming data package
	 */
	void (*new_data)(void* const parent, const data_package_t* const package);
} data_port_t;

/**
 * @brief data_port new data dummy function
 *
 * @param	parent	not used
 * @param	package	not used
 */
void data_port_new_data_dummy(void* const parent,
		const data_package_t* const package);

/**
 * @brief a data_port which can be set when no real data_port is assigned
 */
extern const data_port_t data_port_dummy;

#endif
