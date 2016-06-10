/**
 * @file main.c
 * @brief main source file containing main routine
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <system/peripherals.h>

#include <fpga-log/sys_init.h>
#include <fpga-log/pc_compatibility.h>
#include <fpga-log/datastream_object.h>
#include <fpga-log/datastream_source.h>

#include "logger_config.h"

CLOCK_FREQUENCY_DEFINITION

/**
 * @brief main function
 */
int main() {
#ifdef TIMESTAMP_GEN
	sys_init(CLOCK_FREQUENCY, (timestamp_gen_regs_t*)TIMESTAMP_GEN);
#else
        sys_init(CLOCK_FREQUENCY, 0);
#endif

	init_objects();
	connect_ports();
	advanced_config();

	while (1) {
#ifdef TIMESTAMP_GEN
		datastream_sources_send_data();
#endif
		datastream_objects_update();
	}

	return 0;
}
