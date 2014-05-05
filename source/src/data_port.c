/**
 * @file data_port.c
 * @brief data_port implementations to realize datastreams from devices to sinks
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "data_port.h"

void data_port_new_data_dummy(void* parent, data_package_t* package) {};

const data_port_t data_port_dummy = {0, data_port_new_data_dummy};
