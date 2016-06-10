/**
 * @file data_port.h
 * @brief data_port and data_package structures to realize datastreams from devices to sinks
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DATA_PORT_H_
#define DATA_PORT_H_

#include <peripherals/timestamp_gen.h>

/**
 * @brief enumeration of the different data types which can be in a data package
 */
typedef enum {
	DATA_TYPE_INT = 0, /**< single integer value */
	DATA_TYPE_BYTE = 1, /**< single byte value */
	DATA_TYPE_LONG = 2, /**< long integer value */
	DATA_TYPE_SIMPLE_FLOAT = 3, /**< simple float value, see @ref simple_float_b10_t */
	DATA_TYPE_CHAR = 4, /** (printable) character value */
} data_type;

/**
 * size (in bytes) of the data types defined in @ref data_type
 */
#define DATA_TYPE_BYTE_SIZES {2,1,4,6,1}
extern const int data_type_byte_sizes[];
/**
 * maximum size (in bytes) of the data types defined in @ref data_type
 */
#define DATA_TYPE_MAX_BYTES 6

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
