/**
 * @file datastream_object.h
 * @brief datastream_object as super-class for sinks/devices, update function to update all datastream_objects
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAMS_OBJECT_H_
#define DATASTREAMS_OBJECT_H_

#define MAX_DATASTREAM_OBJECTS 128

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
	 * Everything that does not need to be done immediately in an interrupt is done here.
	 * If the device is a source and uses software timestamps you can generate them in this function.
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
void datastream_objects_update(void);

#endif
