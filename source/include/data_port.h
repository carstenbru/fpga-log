/**
 * @file data_port.h
 * @brief data_port and data_package structures to realize datastreams from devices to sinks
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATA_PORT_H_
#define DATA_PORT_H_

#include <peripherals/timestamp_gen.h>

/**
 * @brief enumeration of the different data types which can be in a data package
 */
typedef enum {
	DATA_TYPE_INT, /**< single integer value */
	DATA_TYPE_BYTE, /**< single byte value */
	DATA_TYPE_LONG, /**< long integer value */
	DATA_TYPE_SIMPLE_FLOAT, /**< simple float value, see @ref simple_float_b10_t */
	DATA_TYPE_CHAR, /** (printable) character value */
} data_type;

/**
 * @brief struct holding incoming data
 */
typedef struct {
	int source_id; /**< the id of the data source, e.g. to show in logs */
	const char* val_name; /**< the name of the value (as string) to differ between multiple values from one source (e.g. channels) */
	data_type type; /**< the type of the data value, should be a value of enumeration @ref data_type */
	const void* data; /**< pointer to the actual data */
	const timestamp_t* timestamp; /**< timestamp of the data */
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
 * @brief a data_port dummy
 *
 * can be set when no real data_port is assigned
 * or before real values are assigned so that no function pointer is null
 */
extern const data_port_t data_port_dummy;

#endif
