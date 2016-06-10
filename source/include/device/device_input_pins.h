/**
 * @file device_input_pins.h
 * @brief simple Pin input device (e.g. to connect switches)
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DEVICE_INPUT_PINS_H_
#define DEVICE_INPUT_PINS_H_

#include <fpga-log/datastream_source.h>

#include <fpga-log/control_port.h>
#include <fpga-log/data_port.h>
#include <peripherals/port_in.h>

/**
 * @brief enumeration of the different control parameters of the input_pins device
 */
typedef enum {
	INPUT_PINS_PARAMETER_CAPTURE = 'c', /**< @value_ref none triggers a data output */
} input_pins_parameter_cpt;

typedef enum {
	INPUT_PINS_EDGE_FALLING = 0, /**< interrupt on the falling edge */
	INPUT_PINS_EDGE_RISING = 1, /**< interrupt on the rising edge */
} input_pins_edge_mode;

/**
 * @brief struct describing a input_pins device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* data_out; /**< data output destination */
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */
	port_in_regs_t* port_in; /**< input port peripheral */
} device_input_pins_t;

/**
 * @brief input_pins device init function
 *
 * Initializes the input_pins device, should be called before using the device.
 *
 * @param input_pins	pointer to the input_pins device
 * @param	int_id			id of the interrupt signal in the timestamp generator
 * @param port_in			input port peripheral
 */
void device_input_pins_init(device_input_pins_t* const input_pins,
		const int int_id, port_in_regs_t* port_in);

/**
 * @brief returns the input_pins device control input
 *
 * @param input_pins		pointer to the input_pins device
 * @return the control input port
 */
control_port_t* device_input_pins_get_control_in(device_input_pins_t* const input_pins);

/**
 * @brief connects the data output port of a input_pins device to a given destination
 *
 * @param	input_pins	pointer to the input_pins device
 * @param	data_in			the new data destination
 */
void device_input_pins_set_data_out(device_input_pins_t* const input_pins,
		const data_port_t* const data_in);

/**
 * @brief enables a pin change interrupt on a specific pin on the requested edge
 *
 * @param input_pins	pointer to the input_pins device
 * @param pin					number of the pin
 * @param edge_sel		edge type to trigger at
 */
void device_input_pins_enable_interrupt(device_input_pins_t* const input_pins,
		unsigned int pin, input_pins_edge_mode edge_sel);

#endif
