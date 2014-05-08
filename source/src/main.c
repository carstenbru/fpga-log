/**
 * @file main.c
 * @brief main source file containing main routine
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <system.h>
#include <system/peripherals.h>
#include <uart.h>

#include "datastream_object.h"
#include "device/device_uart_raw.h"
#include "sink/sink_uart.h"
#include "sink/formatter/formatter_simple.h"
#include "dm/dm_splitter_data.h"
#include "dm/dm_splitter_control.h"
#include "dm/dm_trigger.h"
#include "dm/datastream_condition_compare.h"

#include "pc_native/pc_compatibility.h"

sink_uart_t sink_uart;
formatter_simple_t formatter_simple;

device_uart_raw_t uart_raw;

dm_splitter_data_t splitter_data;
dm_trigger_t trigger;

datastream_condition_compare_t cond;

/**
 * @brief main function
 */
void main() {
	pc_native_init();

	device_uart_raw_init(&uart_raw, UART_LIGHT_PC, 1);

	formatter_simple_init(&formatter_simple);
	sink_uart_init(&sink_uart, (formatter_t*) &formatter_simple, UART_LIGHT_PC);

	dm_splitter_data_init(&splitter_data);

	dm_splitter_data_add_data_out(&splitter_data, &sink_uart.data_in);
	//dm_splitter_data_add_data_out(&splitter_data, &sink_uart.data_in);
	//dm_splitter_data_add_data_out(&splitter_data, &sink_uart.data_in);
	//dm_splitter_data_add_data_out(&splitter_data, &sink_uart.data_in);
	device_uart_raw_set_data_out(&uart_raw, &splitter_data.data_in);  //connect the data_out of uart_raw device to the uart sink

	dm_trigger_init(&trigger);
	dm_trigger_set_control_out(&trigger, &uart_raw.control_in);
	dm_splitter_data_add_data_out(&splitter_data, &trigger.data_in);

	datastream_condition_compare_init(&cond, equal, COMPARE_MODE_VALUE, 66);
	dm_trigger_set_condition(&trigger, (datastream_condition_t*) &cond);

	while (1) {
		datastreams_update();
	}
}
