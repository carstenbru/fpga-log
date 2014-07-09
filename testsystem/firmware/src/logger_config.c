#include "logger_config.h"

#include <system/peripherals.h>
#include <fpga-log/pc_compatibility.h>

#include <fpga-log/datastream_object.h>
#include <fpga-log/datastream_source.h>
#include <fpga-log/device/device_uart_raw.h>
#include <fpga-log/device/device_pwm.h>
#include <fpga-log/device/device_hct99.h>
#include <fpga-log/sink/sink_uart.h>
#include <fpga-log/sink/formatter/formatter_simple.h>
#include <fpga-log/sink/control_protocol_ascii.h>
#include <fpga-log/dm/dm_splitter_data.h>
#include <fpga-log/dm/dm_splitter_control.h>
#include <fpga-log/dm/dm_trigger.h>
#include <fpga-log/dm/datastream_condition_compare_int.h>
#include <fpga-log/dm/datastream_condition_compare_float.h>
#include <fpga-log/dm/datastream_condition_compare_val_name.h>
#include <fpga-log/dm/dm_filter.h>
#include <fpga-log/dm/dm_timer.h>
#include <fpga-log/dm/control_action_simple.h>
#include <fpga-log/sink/sink_sd_card.h>

sink_uart_t sink_uart;
formatter_simple_t formatter_simple, formatter_simple2;
control_protocol_ascii_t control_protocol_ascii;

//device_uart_raw_t uart_raw;

dm_splitter_data_t splitter_data;
dm_trigger_t trigger;
dm_filter_t filter;

datastream_condition_compare_int_t cond;
datastream_condition_compare_float_t cond2;
datastream_condition_compare_val_name_t cond3;

dm_timer_t timer;

sink_sd_card_t sink_sd;

device_pwm_t pwm_dev;
device_hct99_t hct99;

control_action_simple_t c_action;
control_action_simple_t c_action2;

void init_objects(void) {
	device_hct99_init(&hct99, UART_LIGHT_1, 1);
	device_pwm_init(&pwm_dev, PWM_0);

	//device_uart_raw_init(&uart_raw, UART_LIGHT_PC, 0);

	formatter_simple_init(&formatter_simple);
	control_protocol_ascii_init(&control_protocol_ascii);
	sink_uart_init(&sink_uart, (formatter_t*) &formatter_simple,
			(control_protocol_t*) &control_protocol_ascii, UART_LIGHT_PC);

	formatter_simple_init(&formatter_simple2);
	sink_sd_card_init(&sink_sd, (formatter_t*) &formatter_simple2, SDCARD_0);

	dm_splitter_data_init(&splitter_data);

	control_action_simple_init(&c_action);

	control_action_simple_init(&c_action2);

	dm_trigger_init(&trigger, &c_action2);

	datastream_condition_compare_val_name_init(&cond3, RESULT_MODE_EQUAL, "Tc");
	dm_filter_init(&filter, FILTER_MODE_PASS, (datastream_condition_t*) &cond3);

	//datastream_condition_compare_int_init(&cond, equal, COMPARE_MODE_VALUE, 'T');
	simple_float_b10_t f = { 30000, 3 };
	datastream_condition_compare_float_init(&cond2, greater_float, f);

	dm_timer_init(&timer, 1000, &c_action, TIMER_0, COMPARE_0);
}

void connect_ports(void) {
	dm_splitter_data_add_data_out(&splitter_data, &sink_uart.data_in);
	dm_splitter_data_add_data_out(&splitter_data, &sink_sd.data_in);

	device_hct99_set_data_out(&hct99, &splitter_data.data_in);
	device_hct99_set_misc_out(&hct99, &splitter_data.data_in);
	device_hct99_set_error_out(&hct99, &splitter_data.data_in);
	//device_uart_raw_set_data_out(&uart_raw, &splitter_data.data_in);  //connect the data_out of uart_raw device to the uart sink

	dm_splitter_data_add_data_out(&splitter_data, &filter.data_in);

	dm_filter_set_data_out(&filter, &trigger.data_in);
	//dm_trigger_set_control_out(&trigger, &hct99.control_in);

	dm_timer_set_control_out(&timer, &hct99.control_in);

	sink_uart_add_control_out(&sink_uart, &pwm_dev.control_in);
	//sink_uart_add_control_out(&sink_uart, &uart_raw.control_in);
	sink_uart_add_control_out(&sink_uart, device_hct99_get_control_in(&hct99));
}

void advanced_config(void) {
	control_action_simple_add_paramter_exp(&c_action, 'G', 0);
	control_action_simple_add_paramter_exp(&c_action, 'x', 1);
	control_action_simple_add_paramter_exp(&c_action, 'y', 6);

	control_action_simple_add_command(&c_action2);
	control_action_simple_add_paramter_exp(&c_action2, 'G', 0);
	control_action_simple_add_paramter_exp(&c_action2, 'x', 0);
	control_action_simple_add_paramter_exp(&c_action2, 'y', 1);

	control_action_simple_add_command(&c_action2);
	control_action_simple_add_paramter_exp(&c_action2, 'G', 0);
	control_action_simple_add_paramter_exp(&c_action2, 'x', 0);
	control_action_simple_add_paramter_exp(&c_action2, 'y', 2);

	dm_trigger_set_condition(&trigger, (datastream_condition_t*) &cond2);
}
