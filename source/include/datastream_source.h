/**
 * @file datastream_source.h
 * @brief datastream_source as super-class for data sources (devices), send_data function for all datastream_sources
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DATASTREAMS_SOURCE_H_
#define DATASTREAMS_SOURCE_H_

#define MAX_DATASTREAM_SOURCES 64

#include <fpga-log/datastream_object.h>
#include <peripherals/timestamp_gen.h>

/**
 * @brief struct describing a source in the datastreams
 *
 * This is a super-"class" for sources in the datastream (devices)
 */
typedef struct {
	datastream_object_t super; /**< super-"class": datastream_object_t*/
	unsigned int id; /**< source id */

	/**
	 * @brief send data function of the datastream source
	 *
	 * This function should send the data packages to the devices output port.
	 *
	 * @param	datastream_source		pointer to the datastream_source struct
	 * @param id									the source id of the pending data
	 * @param timestamp						pointer to the timestamp of the pending data
	 */
	void (*send_data)(void* const datastream_source, const unsigned int id,
			const timestamp_t* const timestamp);
} datastream_source_t;

/**
 * @brief initializes a datastream_source_t and sets the id for send_data calls
 *
 * For multiple id's call this function with each id.
 *
 * @param source	the datastream_source to initialize
 * @param id			id of the datastream source, has to match the line id in the timestamp generator
 * @return 1 on success, otherwise 0 (illegal id)
 */
int datastream_source_init(datastream_source_t* const source,
		const unsigned int id);

/**
 * @brief generates a software triggered timestamp in the timestamp generator
 *
 * When this function is called a timestamp is gerated and
 * later on the send_data function of the device is invoked with this timestamp.
 *
 * @param source
 */
void datastream_source_generate_software_timestamp(datastream_source_t* const source);

/**
 * @brief	calls the send_data functions of all datastream sources with pending data
 */
void datastream_sources_send_data(void);

#endif
