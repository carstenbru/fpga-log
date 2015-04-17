/**
 * @file main.c
 * @brief main source file containing main routine
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <system/peripherals.h>

#include <fpga-log/sys_init.h>
#include <fpga-log/pc_compatibility.h>
#include <fpga-log/datastream_object.h>
#include <fpga-log/datastream_source.h>

#include "logger_config.h"

/**
 * @brief main function
 */
int main() {
	sys_init(16000000, (timestamp_gen_regs_t*)TIMESTAMP_GEN);

	init_objects();
	connect_ports();
	advanced_config();

	while (1) {
		datastream_sources_send_data();
		datastream_objects_update();
	}

	return 0;
}
