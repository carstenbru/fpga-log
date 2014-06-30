/**
 * @file data_port.c
 * @brief data_port implementations to realize datastreams from devices to sinks
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/data_port.h>

/**
 * @brief data_port new data dummy function
 *
 * @param	parent	not used
 * @param	package	not used
 */
static void data_port_new_data_dummy(void* const parent, const data_package_t* const package) {};

const data_port_t data_port_dummy = {0, data_port_new_data_dummy};
