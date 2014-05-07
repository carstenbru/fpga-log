/**
 * @file datastreams.h
 * @brief datastream_object as super-class for sinks/devices, update function to update all datastream_objects
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAMS_H_
#define DATASTREAMS_H_

#define MAX_DATASTREAM_OBJECTS 64

/**
 * @brief struct describing an object in the datastreams
 *
 * This is a super-"class" for objects in the datastream
 * including sinks,devices and other data stream modules (short: dm).
 */
typedef struct {
	/**
	 * @brief update function of the datastream object
	 *
	 * everything that does not need to be done immediately in an interrupt is done here, e.g. sending data to a data_out port
	 *
	 * @param	datastream_object		pointer to the datastream_object struct
	 */
	void (*update)(void* const datastream_object);
} datastream_object_t;

/**
 * @brief initializes a datastream_object_t
 *
 * @param object	the datastream_object to initialize
 */
void datastream_object_init(datastream_object_t* const object);

/**
 * @brief	updates all datastream_object_t's
 */
void datastreams_update(void);

#endif
