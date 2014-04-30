/**
 * @file data_port.h
 * @brief data_port and data_package structures to realize datastreams from devices to sinks
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */


#ifndef DATA_PORT_H_
#define DATA_PORT_H_
 
/**
 * @brief enumeration of the diffrent data types which can be in a data package
 */
typedef enum { 
  DATA_TYPE_INT,	/**< single integer value */
  DATA_TYPE_BYTE 	/**< single byte value */
} data_type;

/**
 * @brief struct holding incoming data
 */
typedef struct {
  int source_id;	/**< the id of the data source, e.g. to show in logs */
  int type;		/**< the type of the data value, should be a value of enumeration @ref data_type */
  void* data;		/**< pointer to the actual data */
} data_package_t;

/**
 * @brief struct defining a data destination
 */
typedef struct {
  void* parent;					/**< pointer to the parent struct, e.g. a datasink module */
  void (*new_data)(void*, data_package_t*);	/**< pointer to the new-data-function of the destination, should be called with parent as parameter */
} data_port_t;

#endif