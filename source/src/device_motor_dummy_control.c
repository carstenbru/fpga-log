#include "fpga-log/device/device_motor_dummy_control.h"

static void device_motor_dummy_control_send_data(
		void* const _motor_dummy_control, const unsigned int id,
		const timestamp_t* const timestamp);
static void device_motor_dummy_control_new_control_message(
		void* const _motor_dummy_control, unsigned int count,
		const control_parameter_t* parameters);

void device_motor_dummy_control_init(
		device_motor_dummy_control_t* const motor_dummy_control, int id,
		motor_dummy_control_regs_t* motor_dummy_control_peri) {
	datastream_source_init(&motor_dummy_control->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	motor_dummy_control->super.send_data = device_motor_dummy_control_send_data;

	motor_dummy_control->motor_dummy_control = motor_dummy_control_peri;

	motor_dummy_control->data_request_out = &data_port_dummy;
	motor_dummy_control->data_interrupt_out = &data_port_dummy;

	motor_dummy_control->control_in = control_port_dummy;
	motor_dummy_control->control_in.parent = motor_dummy_control;
	motor_dummy_control->control_in.new_control_message =
			device_motor_dummy_control_new_control_message;

	motor_dummy_control->was_requested = 0;
	motor_dummy_control->fpga_id = id;
}

void device_motor_dummy_control_set_data_request_out(
		device_motor_dummy_control_t* const motor_dummy_control,
		const data_port_t* const data_out) {
	motor_dummy_control->data_request_out = data_out;
}
void device_motor_dummy_control_set_data_interrupt_out(
		device_motor_dummy_control_t* const motor_dummy_control,
		const data_port_t* const data_out) {
	motor_dummy_control->data_interrupt_out = data_out;
}

control_port_t* device_motor_dummy_control_get_control_in(
		device_motor_dummy_control_t* const motor_dummy_control) {
	return &motor_dummy_control->control_in;
}

static void device_motor_dummy_control_send_data(
		void* const _motor_dummy_control, const unsigned int id,
		const timestamp_t* const timestamp) {
	device_motor_dummy_control_t* motor_dummy_control =
			(device_motor_dummy_control_t*) _motor_dummy_control;

	unsigned char state = motor_dummy_control->motor_dummy_control->control;
	data_package_t package = { motor_dummy_control->fpga_id, "dummy status",
			DATA_TYPE_BYTE, (void*) &state, timestamp };  //use source_id field for FPGA ID..but that's not the normal intention of this field!
	if (motor_dummy_control->was_requested) {
		motor_dummy_control->data_request_out->new_data(
				motor_dummy_control->data_request_out->parent, &package);
	} else {
		motor_dummy_control->data_interrupt_out->new_data(
				motor_dummy_control->data_interrupt_out->parent, &package);
	}
	motor_dummy_control->was_requested = 0;
}

static void device_motor_dummy_control_new_control_message(
		void* const _motor_dummy_control, unsigned int count,
		const control_parameter_t* parameters) {
	device_motor_dummy_control_t* motor_dummy_control =
			(device_motor_dummy_control_t*) _motor_dummy_control;

	while (count--) {
		switch (parameters->type) {
		case MOTOR_DUMMY_CONTROL_COMMAND_SET:
			motor_dummy_control->motor_dummy_control->control = (parameters->value
					!= 0);
			break;
		case MOTOR_DUMMY_CONTROL_COMMAND_REQUEST:
			motor_dummy_control->was_requested = 1;
			_datastream_source_generate_software_timestamp(
					(datastream_source_t*) motor_dummy_control);
			break;
		case MOTOR_DUMMY_CONTROL_COMMAND_SET_FPGA_ID:
			motor_dummy_control->fpga_id = parameters->value;
			break;
		}
		parameters++;
	}
}
