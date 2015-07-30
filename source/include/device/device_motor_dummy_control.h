#ifndef DEVICE_MOTOR_DUMMY_CONTROL_H_
#define DEVICE_MOTOR_DUMMY_CONTROL_H_

#include <fpga-log/control_port.h>
#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>

#include <peripherals/motor_dummy_control.h>

typedef enum {
	MOTOR_DUMMY_CONTROL_COMMAND_SET = 'd',
	MOTOR_DUMMY_CONTROL_COMMAND_REQUEST = 's',
	MOTOR_DUMMY_CONTROL_COMMAND_SET_FPGA_ID = 'j',
} device_motor_dummy_control_cpt;

typedef struct {
	datastream_source_t super; /**< super-"class": datastream_object_t*/

	control_port_t control_in; /**< control input port */
	const data_port_t* data_request_out; /**< data output (on request) destination */
	const data_port_t* data_interrupt_out; /**< data output (on interrupt) destination */

	int was_requested;
	motor_dummy_control_regs_t* motor_dummy_control;

	int fpga_id;
} device_motor_dummy_control_t;

void device_motor_dummy_control_init(device_motor_dummy_control_t* const motor_dummy_control,
		int id, motor_dummy_control_regs_t* motor_dummy_control_peri);

void device_motor_dummy_control_set_data_request_out(
		device_motor_dummy_control_t* const motor_dummy_control,
		const data_port_t* const data_out);
void device_motor_dummy_control_set_data_interrupt_out(
		device_motor_dummy_control_t* const motor_dummy_control,
		const data_port_t* const data_out);
control_port_t* device_motor_dummy_control_get_control_in(
		device_motor_dummy_control_t* const motor_dummy_control);

#endif
