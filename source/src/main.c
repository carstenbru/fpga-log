/**
 * @file main.c
 * @brief main source file containing main routine
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <system.h>
#include <system/peripherals.h>
#include <uart.h>

#include "peripheral_funcs/pwm.h"

#include "sys_init.h"

#include "datastream_object.h"
#include "device/device_uart_raw.h"
#include "device/device_pwm.h"
#include "sink/sink_uart.h"
#include "sink/formatter/formatter_simple.h"
#include "sink/control_protocol_ascii.h"
#include "dm/dm_splitter_data.h"
#include "dm/dm_splitter_control.h"
#include "dm/dm_trigger.h"
#include "dm/datastream_condition_compare.h"
#include "dm/dm_filter.h"
#include "dm/dm_timer.h"

#include "pc_native/pc_compatibility.h"

#include "FatFs/ff.h"
#include "sink/sink_sd_card.h"

sink_uart_t sink_uart;
formatter_simple_t formatter_simple, formatter_simple2;
control_protocol_ascii_t control_protocol_ascii;

device_uart_raw_t uart_raw;

dm_splitter_data_t splitter_data;
dm_trigger_t trigger;

datastream_condition_compare_t cond;

dm_timer_t timer;

sink_sd_card_t sink_sd;

device_pwm_t pwm_dev;

/**
 * @brief main function
 */
int main() {
	sys_init();

	device_pwm_init(&pwm_dev, PWM_0);

	device_uart_raw_init(&uart_raw, UART_LIGHT_1, 1);

	formatter_simple_init(&formatter_simple);
	control_protocol_ascii_init(&control_protocol_ascii);
	sink_uart_init(&sink_uart, (formatter_t*) &formatter_simple,
			(control_protocol_t*) &control_protocol_ascii, UART_LIGHT_PC);

	formatter_simple_init(&formatter_simple2);
	sink_sd_card_init(&sink_sd, (formatter_t*) &formatter_simple2, SDCARD_0);

	dm_splitter_data_init(&splitter_data);

	dm_splitter_data_add_data_out(&splitter_data, &sink_uart.data_in);
	dm_splitter_data_add_data_out(&splitter_data, &sink_sd.data_in);

	device_uart_raw_set_data_out(&uart_raw, &splitter_data.data_in);  //connect the data_out of uart_raw device to the uart sink

	dm_trigger_init(&trigger);
	dm_trigger_set_control_out(&trigger, &uart_raw.control_in);
	dm_splitter_data_add_data_out(&splitter_data, &trigger.data_in);

	datastream_condition_compare_init(&cond, equal, COMPARE_MODE_VALUE, 'B');
	dm_trigger_set_condition(&trigger, (datastream_condition_t*) &cond);

	dm_timer_init(&timer, 1000, TIMER_0, COMPARE_0);
	dm_timer_set_control_out(&timer, &uart_raw.control_in);

	sink_uart_add_control_out(&sink_uart, &pwm_dev.control_in);

	while (1) {
		datastreams_update();
	}

	return 0;
}
