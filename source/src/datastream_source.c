/**
 * @file datastream_source.c
 * @brief datastream_source as super-class for data sources (devices), send_data function for all datastream_sources
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "datastream_source.h"
#include <peripherals/timestamp_gen.h>
#include <system/peripherals.h>
#include "pc_native/pc_compatibility.h"

datastream_source_t* datastream_source_list[MAX_DATASTREAM_SOURCES];

void datastream_sources_send_data(void) {
	datastream_source_t* source;
	while (TIMESTAMP_GEN_NOT_EMPTY(TIMESTAMP_GEN)) {
		if (TIMESTAMP_GEN->tsr < MAX_DATASTREAM_SOURCES) {
			source = datastream_source_list[TIMESTAMP_GEN->tsr];
			if (source != 0) {
				source->send_data(source, TIMESTAMP_GEN->tsr,
						&TIMESTAMP_GEN->timestamp);
			}
		}
		TIMESTAMP_GEN->control = TIMESTAMP_GEN_NEXT_VAL;
	}
}

int datastream_source_init(datastream_source_t* const source,
		const unsigned int id) {
	datastream_object_init((datastream_object_t*) source);

	if (id < MAX_DATASTREAM_SOURCES) {
		datastream_source_list[id] = source;
		source->id = id;
		return 1;
	} else
		return 0;
}

void datastream_source_generate_software_timestamp(
		datastream_source_t* const source) {
	TIMESTAMP_GEN_GENERATE_SOFTWARE_TIMESTAMP(TIMESTAMP_GEN, source->id);
}
