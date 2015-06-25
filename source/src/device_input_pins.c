/**
 * @file device_input_pins.c
 * @brief simple Pin input device (e.g. to connect switches)
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_input_pins.h>

/**
 * @brief input_pins send data function
 *
 * This function sends the data packages to the devices output port.
 *
 * @param	_input_pins		pointer to the input_pins device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_input_pins_send_data(void* const _input_pins,
		const unsigned int id, const timestamp_t* const timestamp);

/**
 * @brief control message function of input_pins device
 *
 * @param	parent			the input_pins device
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void device_input_pins_control_message(void* const _input_pins,
		unsigned int count, const control_parameter_t* parameters);

void device_input_pins_init(device_input_pins_t* const input_pins,
		const int int_id, port_in_regs_t* port_in) {
	datastream_source_init(&input_pins->super, int_id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	input_pins->super.send_data = device_input_pins_send_data;

	input_pins->data_out = &data_port_dummy;
	input_pins->control_in = control_port_dummy;
	input_pins->control_in.parent = (void*) input_pins;
	input_pins->control_in.new_control_message =
			device_input_pins_control_message;

	input_pins->port_in = port_in;
}

control_port_t* device_input_pins_get_control_in(
		device_input_pins_t* const input_pins) {
	return &input_pins->control_in;
}

void device_input_pins_set_data_out(device_input_pins_t* const input_pins,
		const data_port_t* const data_in) {
	input_pins->data_out = data_in;
}

static void device_input_pins_send_data(void* const _input_pins,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_input_pins_t* input_pins = (device_input_pins_t*) _input_pins;

	unsigned int value = input_pins->port_in->data;
	data_package_t package = { id, "data", DATA_TYPE_INT, &value, timestamp };
	input_pins->data_out->new_data(input_pins->data_out->parent, &package);
}

static void device_input_pins_control_message(void* const _input_pins,
		unsigned int count, const control_parameter_t* parameters) {
	device_input_pins_t* input_pins = (device_input_pins_t*) _input_pins;

	while (count--) {
		if (parameters->type == INPUT_PINS_PARAMETER_CAPTURE) {
			_datastream_source_generate_software_timestamp(
					(datastream_source_t*) input_pins);
		}
		parameters++;
	}
}

void device_input_pins_enable_interrupt(device_input_pins_t* const input_pins,
		unsigned int pin, input_pins_edge_mode edge_sel) {
	input_pins->port_in->ie |= (1 << pin);
	input_pins->port_in->edgsel |= (edge_sel << pin);
}
