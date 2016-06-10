/**
 * @file device_hct99.c
 * @brief HCT-99 optometer device driver
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/device/device_hct99.h>
#include <fpga-log/peripheral_funcs/uart_light_funcs.h>

/**
 * @brief checks if a byte is a ASCII digit
 */
#define IS_DIGIT(b) (b >= '0' && b <= '9')

const char* hct99_names_channel[4] = HCT99_NAMES_CHANNEL;
const char* hct99_names_transmission_reflection[4] =
HCT99_NAMES_TRANSMISSION_REFLECTION;
const char* hct99_names_color_1[9] = HCT99_NAMES_COLOR_1;
const char* hct99_names_color_2[10] = HCT99_NAMES_COLOR_2;
const char* hct99_names_color_2_alt[4] = HCT99_NAMES_COLOR_2_ALT;
const char* hct99_names_color_3[4] = HCT99_NAMES_COLOR_3;

/**
 * @brief HCT-99 send data function
 *
 * This function sends the data packages to the devices output port.
 *
 * @param	_hct99			pointer to the HCT-99 device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_hct99_send_data(void* const _hct99, const unsigned int id,
		const timestamp_t* const timestamp);

/**
 * @brief sends the next command in the fifo queue to the HCT-99 device
 *
 * @param hct99		pointer to the HCT-99 device
 */
static void device_hct99_send_command(device_hct99_t* const hct99);

/**
 * @brief control message function of the HCT-99 device
 *
 * @param	_hct99			the HCT-99 device
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void device_hct99_new_control_message(void* const _hct99,
		unsigned int count, const control_parameter_t* parameters);

/**
 * @brief sends the parameters to the HCT-99 device
 *
 * @param hct99											pointer to the HCT-99 device
 * @param count											number of parameters to send
 * @param params										array of the actual parameters
 * @param last_param_three_digits		0 for normal parameter send, 1 if the last parameter should contain 3 digits
 */
static void device_hct99_send_paramters(device_hct99_t* const hct99,
		unsigned int count, const unsigned int* params,
		unsigned char last_param_three_digits);

/**
 * @brief update function of the HCT-99 device
 *
 * @param _hct99	pointer to the HCT-99 device
 */
static void device_hct99_update(void* const _hct99);

void device_hct99_init(device_hct99_t* const hct99,
		uart_light_regs_t* const uart_light, const int id) {
	datastream_source_init(&hct99->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	hct99->super.super.update = device_hct99_update;
	hct99->super.send_data = device_hct99_send_data;

	hct99->data_out = &data_port_dummy;
	hct99->misc_out = &data_port_dummy;
	hct99->error_out = &data_port_dummy;

	hct99->control_in = control_port_dummy;
	hct99->control_in.parent = (void*) hct99;
	hct99->control_in.new_control_message = device_hct99_new_control_message;

	hct99->uart_light = uart_light;

	uart_light_enable_rxint(uart_light);

	hct99->expected_byte = HCT99_EXPECT_NOTHING;
	hct99->command_fifo_elements = 0;
}

control_port_t* device_hct99_get_control_in(device_hct99_t* const hct99) {
	return &hct99->control_in;
}

void device_hct99_set_data_out(device_hct99_t* const hct99,
		const data_port_t* const data_in) {
	hct99->data_out = data_in;
}

void device_hct99_set_error_out(device_hct99_t* const hct99,
		const data_port_t* const data_in) {
	hct99->error_out = data_in;
}

void device_hct99_set_misc_out(device_hct99_t* const hct99,
		const data_port_t* const data_in) {
	hct99->misc_out = data_in;
}

static void device_hct99_send_data(void* const _hct99, const unsigned int id,
		const timestamp_t* const timestamp) {
	device_hct99_t* hct99 = (device_hct99_t*) _hct99;

	unsigned char byte = 0;
	if (uart_light_receive_nb(hct99->uart_light, &byte) == UART_OK) {
		switch (hct99->expected_byte) {
		case (HCT99_EXPECT_LF): {
			if (byte == '\n') {
				hct99->expected_byte = HCT99_EXPECT_NOTHING;
			}else if (byte == '*'){ //an error occur
				hct99->expected_byte = HCT99_EXPECT_ERR_LF;
			}
			break;
		}
		case (HCT99_EXPECT_COEFFICIENT): {
			if (IS_DIGIT(byte)) {
				hct99->value.coefficient *= 10;
				hct99->value.coefficient += byte - '0';
			} else if (byte == '-') {
				hct99->val_neg = 1;
			} else if (byte == 'E') {
				hct99->expected_byte = HCT99_EXPECT_EXPONENT_1;
				if (hct99->val_neg) {
					hct99->value.coefficient = -hct99->value.coefficient;
					hct99->val_neg = 0;
				}
			}
			break;
		}
		case (HCT99_EXPECT_EXPONENT_1): {
			if (IS_DIGIT(byte)) {
				hct99->value.exponent = (byte - '0') * 10;
				hct99->expected_byte = HCT99_EXPECT_EXPONENT_2;
			} else if (byte == '-') {
				hct99->val_neg = 1;
			}
			break;
		}
		case (HCT99_EXPECT_EXPONENT_2): {
			hct99->value.exponent += (byte - '0');
			hct99->expected_byte = HCT99_EXPECT_ERR_CODE;
			if (hct99->val_neg) {
				hct99->value.exponent = -hct99->value.exponent;
			}
			break;
		}
		case (HCT99_EXPECT_ERR_LF): {
			if(byte == '\n'){
				hct99->command_fifo_elements++; //Resend the last command
				hct99->command_fifo_top--;
				device_hct99_send_command(hct99);
				hct99->expected_byte = HCT99_EXPECT_LF;
			}
			break;
		}
		case (HCT99_EXPECT_ERR_CODE): {
			if (byte == '\n') {
				hct99->expected_byte = HCT99_EXPECT_NOTHING;

				simple_float_b10_t value_saved = hct99->value;
				data_package_t package = { id, hct99->val_name, DATA_TYPE_SIMPLE_FLOAT,
						&value_saved, timestamp };  //TODO use timestamp of first byte?

				//call update function immediately after response receive so a new command can be issued as fast as possible
				device_hct99_update(hct99);

				hct99->data_out->new_data(hct99->data_out->parent, &package);

				if (hct99->err_code != HCT99_ERROR_CODE_OK) {
					data_package_t package = { id, HCT99_ERROR_DEVICE, DATA_TYPE_BYTE,
							&hct99->err_code, timestamp };
					hct99->error_out->new_data(hct99->error_out->parent, &package);
				}
			} else {
				if (IS_DIGIT(byte)) {
					hct99->err_code *= 10;
					hct99->err_code += (byte - '0');
				}
			}
			break;
		}
		case (HCT99_EXPECT_NUMBER): {
			if (byte == '\n') {
				data_package_t package = { id, hct99->val_name, DATA_TYPE_INT,
						&hct99->value.coefficient, timestamp };
				hct99->misc_out->new_data(hct99->misc_out->parent, &package);

				hct99->expected_byte = HCT99_EXPECT_NOTHING;
			} else {
				if (IS_DIGIT(byte)) {
					hct99->value.coefficient *= 10;
					hct99->value.coefficient += (byte - '0');
				}
			}
			break;
		}
		case (HCT99_EXPECT_NOTHING): {
			data_package_t package = { id, HCT99_ERROR_UNEXPECTED_DATA_RECEIVED,
					DATA_TYPE_BYTE, &byte, timestamp };
			hct99->error_out->new_data(hct99->error_out->parent, &package);
			break;
		}
		}
	} else {  //software generated a timestamp due to an error
		data_package_t package = { id, hct99->err_name, DATA_TYPE_BYTE, &byte,
				timestamp };
		hct99->error_out->new_data(hct99->error_out->parent, &package);
	}
}

static void device_hct99_new_control_message(void* const _hct99,
		unsigned int count, const control_parameter_t* parameters) {
	device_hct99_t* hct99 = (device_hct99_t*) _hct99;

	hct99_command_cpt command = 0;
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;
	unsigned int val = 0;
	unsigned int only_when_empty = 0;

	while (count--) {
		switch (parameters->type) {
		case 'x':
			x = parameters->value;
			break;
		case 'y':
			y = parameters->value;
			break;
		case 'z':
			z = parameters->value;
			break;
		case HCT99_CPARAMETER_ONLY_WHEN_QUEUE_EMPTY:
			only_when_empty = 1;
			break;
		default:
			command = parameters->type;
			val = parameters->value;
			break;
		}
		parameters++;
	}

//re-map measurement time commands
	if (command == HCT99_COMMAND_MEASUREMENT_TIME_SAVE) {
		command = HCT99_COMMAND_MEASUREMENT_TIME;
		y = x;
		x = 2;
	}
	if (command == HCT99_COMMAND_MEASUREMENT_TIME_LOAD) {
		command = HCT99_COMMAND_MEASUREMENT_TIME;
		y = x;
		x = 3;
	}
	if ((command == HCT99_COMMAND_MEASUREMENT_TIME) && val) {
		y = x;
		x = val;
	}

	if (command) {  //check if a command was set
		if ((only_when_empty == 0) || (hct99->command_fifo_elements == 0)) {
			device_hct99_execute_command(hct99, command, x, y, z);
		}
	} else {
		hct99->err_name = HCT99_ERROR_MISSING_COMMAND_CODE;
		_datastream_source_generate_software_timestamp(
				(datastream_source_t*) hct99);
	}
}

static void device_hct99_send_paramters(device_hct99_t* const hct99,
		unsigned int count, const unsigned int* params,
		unsigned char last_param_three_digits) {

	while (count--) {
		unsigned int val = *params++;
		if (!last_param_three_digits || count) {
			if (val < 10) {
				uart_light_send(hct99->uart_light, val + '0');
			} else {
				hct99->err_name = HCT99_ERROR_PARAM_OUT_OF_RANGE;
				_datastream_source_generate_software_timestamp(
						(datastream_source_t*) hct99);
			}
		} else {
			if (val < 1000) {
				uart_light_send(hct99->uart_light, (val / 100) + '0');
				uart_light_send(hct99->uart_light, ((val % 100) / 10) + '0');
				uart_light_send(hct99->uart_light, (val % 10) + '0');
			} else {
				hct99->err_name = HCT99_ERROR_PARAM_OUT_OF_RANGE;
				_datastream_source_generate_software_timestamp(
						(datastream_source_t*) hct99);
			}
		}
	}
}

void device_hct99_execute_command(device_hct99_t* const hct99,
		hct99_command_cpt command_code, unsigned int x, unsigned int y,
		unsigned int z) {

	if (hct99->command_fifo_elements < HCT99_COMMAND_FIFO_SIZE) {
		int i = hct99->command_fifo_top + hct99->command_fifo_elements++;
		i %= HCT99_COMMAND_FIFO_SIZE;
		hct99_command_t command = { command_code, x, y, z };
		hct99->command_fifo[i] = command;
	} else {
		hct99->err_name = HCT99_ERROR_COMMAND_FIFO_FULL;
		_datastream_source_generate_software_timestamp(
				(datastream_source_t*) hct99);
	}
}

static void device_hct99_send_command(device_hct99_t* const hct99) {
	hct99_command_t* command = &hct99->command_fifo[hct99->command_fifo_top++];
	hct99->command_fifo_top %= HCT99_COMMAND_FIFO_SIZE;
	hct99->command_fifo_elements--;

	uart_light_send(hct99->uart_light, command->command_code);

	hct99->expected_byte = HCT99_EXPECT_LF;
	hct99->value.coefficient = 0;
	hct99->value.exponent = 0;
	hct99->err_code = HCT99_ERROR_CODE_OK;
	hct99->val_name = "";
	hct99->val_neg = 0;

	const unsigned int* params = &command->x;
	switch (command->command_code) {
	case HCT99_COMMAND_MEASURE_SINGLE_CHANNEL:
		device_hct99_send_paramters(hct99, 1, params, 0);
		hct99->val_name = hct99_names_channel[command->x];
		hct99->expected_byte = HCT99_EXPECT_COEFFICIENT;
		break;
	case HCT99_COMMAND_MEASURE_COLOR_VALUE:
		device_hct99_send_paramters(hct99, 2, params, 0);
		hct99->expected_byte = HCT99_EXPECT_COEFFICIENT;
		if (command->x < 2) {
			if (command->y) {
				hct99->val_name = hct99_names_color_1[command->y - 1];
			} else {
				hct99->expected_byte = HCT99_EXPECT_NOTHING;
			}
		} else if (command->x == 2) {
			hct99->val_name = hct99_names_color_2[command->y];

			switch (command->y) {
			case (3):
				hct99->color_alternative = 0;
				break;
			case (4):
				hct99->color_alternative = 1;
				break;
			case (5):
			case (6):
			case (8):
				if (hct99->color_alternative) {
					hct99->val_name = hct99_names_color_2_alt[command->y - 5];
				}
				break;
			case (9):
				device_hct99_send_paramters(hct99, 1, &command->z, 1);
				hct99->expected_byte = HCT99_EXPECT_NOTHING;
				break;
			default:
				break;
			}
		} else {
			hct99->val_name = hct99_names_color_3[command->y];
		}
		break;
	case HCT99_COMMAND_MEASURE_TRANSMISSION_REFLECTION:
		device_hct99_send_paramters(hct99, 2, params, 0);
		if (command->y) {
			hct99->val_name = hct99_names_transmission_reflection[command->y - 1];
			hct99->expected_byte = HCT99_EXPECT_COEFFICIENT;
		}
		break;
	case HCT99_COMMAND_MEASURE_SPECIAL_CALCULATION:
		device_hct99_send_paramters(hct99, 2, params, 1);
		hct99->val_name = "special calculation";
		hct99->expected_byte = HCT99_EXPECT_COEFFICIENT;
		break;
	case HCT99_COMMAND_RANGE:
		device_hct99_send_paramters(hct99, 2, params, 0);
		break;
	case HCT99_COMMAND_AUTORANGE:
		device_hct99_send_paramters(hct99, 2, params, 0);
		break;
	case HCT99_COMMAND_MEASUREMENT_TIME:
		device_hct99_send_paramters(hct99, 2, params, command->x < 2);
		break;
	case HCT99_COMMAND_OFFSET:
		device_hct99_send_paramters(hct99, 1, params, 0);
		break;
	case HCT99_COMMAND_MEASUREMENT_MODE:
		device_hct99_send_paramters(hct99, 1, params, 0);
		break;
	case HCT99_COMMAND_ZERO_ADJUST:
		break;
	case HCT99_COMMAND_CAL_DATA_SELECT:
		device_hct99_send_paramters(hct99, 2, params, 1);
		break;
	case HCT99_COMMAND_CAL_DATA_TEXT_INFO:
		device_hct99_send_paramters(hct99, 1, params, 0);
		break;
	case HCT99_COMMAND_SPECIAL_DEV_CODE:
		hct99->val_name = "special device code";
		hct99->expected_byte = HCT99_EXPECT_NUMBER;
		break;
	case HCT99_COMMAND_FIRMWARE_VERSION:
		hct99->val_name = "firmware version";
		hct99->expected_byte = HCT99_EXPECT_NUMBER;
		break;
	case HCT99_COMMAND_FIRMWARE_TYPE:
		hct99->val_name = "firmware type";
		hct99->expected_byte = HCT99_EXPECT_NUMBER;
		break;
	case HCT99_COMMAND_HARDWARE_TYPE:
		hct99->val_name = "hardware type";
		hct99->expected_byte = HCT99_EXPECT_NUMBER;
		break;
	default:  //TODO support 'n' command?
		break;
	}
	uart_light_send(hct99->uart_light, '\n');
}

static void device_hct99_update(void* const _hct99) {
	device_hct99_t* hct99 = (device_hct99_t*) _hct99;

	if (hct99->command_fifo_elements
			&& (hct99->expected_byte == HCT99_EXPECT_NOTHING)) {
		device_hct99_send_command(hct99);
	}
}
