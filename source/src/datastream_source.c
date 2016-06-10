/**
 * @file datastream_source.c
 * @brief datastream_source as super-class for data sources (devices), send_data function for all datastream_sources
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/datastream_source.h>
#include <peripherals/timestamp_gen.h>
#include <fpga-log/pc_compatibility.h>
#include <fpga-log/sys_init.h>

datastream_source_t* datastream_source_list[MAX_DATASTREAM_SOURCES];

void datastream_sources_send_data(void) {
	timestamp_gen_regs_t* timestamp_gen = get_timestamp_gen();

	datastream_source_t* source;
	while (TIMESTAMP_GEN_NOT_EMPTY(timestamp_gen)) {
		if (timestamp_gen->tsr < MAX_DATASTREAM_SOURCES) {
			source = datastream_source_list[timestamp_gen->tsr];
			if (source != 0) {
				source->send_data(source, timestamp_gen->tsr,
						(const timestamp_t* const)&timestamp_gen->timestamp);
			}
		}
		timestamp_gen->control = TIMESTAMP_GEN_NEXT_VAL;
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

void _datastream_source_generate_software_timestamp(
		datastream_source_t* const source) {
	TIMESTAMP_GEN_GENERATE_SOFTWARE_TIMESTAMP(get_timestamp_gen(), source->id);
}
