/**
 * @file device_PANdrive.c
 * @brief device driver (UART) for PANdrive motors of trinamic
 *
 * The command code of commands on the control port can be either specified by using the value field
 * of the 'c'-parameter type or by using the command code directly as parameter type and the value
 * field of this parameter for the real value.
 * For controlling by a PC via an ASCII protocol the usage of the 'c'-parameter is recommended, for
 * internal control commands using the command code as parameter type is recommended.
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/device/device_PANdrvie.h>
#include <fpga-log/peripheral_funcs/uart_light_funcs.h>
#include <fpga-log/long_int.h>
#include <stdio.h>

const char* pan_drive_error_codes[6] = PAN_DRIVE_ERROR_CODES;

static void device_PANdrive_send_data(void* const _pan_drive,
		const unsigned int id, const timestamp_t* const timestamp);
static void device_PANdrive_new_control_message(void* const _pan_drive,
		unsigned int count, const control_parameter_t* parameters);

void device_PANdrive_init(device_PANdrive_t* const pan_drive,
		uart_light_regs_t* const uart_light, const int id) {
	datastream_source_init(&pan_drive->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	pan_drive->super.send_data = device_PANdrive_send_data;

	pan_drive->data_out = &data_port_dummy;
	pan_drive->status_out = &data_port_dummy;

	pan_drive->control_in = control_port_dummy;
	pan_drive->control_in.parent = (void*) pan_drive;
	pan_drive->control_in.new_control_message =
			device_PANdrive_new_control_message;

	pan_drive->uart_light = uart_light;

	uart_light_enable_rxint(uart_light);

	pan_drive->target_address = PAN_DRIVE_DEFAULT_TARGET_ADDRESS;
	pan_drive->receive_state = 0;
	pan_drive->received_status = 0;
}

control_port_t* device_PANdrive_get_control_in(
		device_PANdrive_t* const pan_drive) {
	return &pan_drive->control_in;
}

void device_PANdrive_set_data_out(device_PANdrive_t* const pan_drive,
		const data_port_t* const data_in) {
	pan_drive->data_out = data_in;
}

void device_PANdrive_set_status_out(device_PANdrive_t* const pan_drive,
		const data_port_t* const data_in) {
	pan_drive->status_out = data_in;
}

static void device_PANdrive_buffer_printf(void* param, const unsigned char byte) {
	device_PANdrive_t* pan_drive = (device_PANdrive_t*) param;
	*pan_drive->buf++ = byte;
}

static void device_PANdrive_send_data(void* const _pan_drive,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_PANdrive_t* pan_drive = (device_PANdrive_t*) _pan_drive;

	unsigned char byte;
	if (uart_light_receive_nb(pan_drive->uart_light, &byte) == UART_OK) {
		pan_drive->received_value <<= 8;
		pan_drive->received_value |= cast_to_ulong(byte);

		switch (pan_drive->receive_state) {
		case 2:
			pan_drive->received_status = byte;
			break;
		case 3: {
			pan_drive->received_value = 0;
			pan_drive->reply_command_code = byte;

			const char* val_name = "successfully executed";
			if (pan_drive->received_status <= 6) {
				val_name = pan_drive_error_codes[pan_drive->received_status - 1];
			}
			data_package_t package =
					{ id, val_name, DATA_TYPE_BYTE, &byte, timestamp };
			pan_drive->status_out->new_data(pan_drive->status_out->parent, &package);
			break;
		}
		case 7: {
			char buffer[32];
			pan_drive->buf = buffer;
			stdio_descr.base_addr_out = pan_drive;
			stdio_descr.send_byte = device_PANdrive_buffer_printf;
			printf("command %d", pan_drive->reply_command_code);
			*pan_drive->buf = 0;

			data_package_t package = { id, buffer, DATA_TYPE_LONG,
					&pan_drive->received_value, timestamp };
			pan_drive->data_out->new_data(pan_drive->data_out->parent, &package);
			break;
		}
		default:
			break;
		}

		pan_drive->receive_state++;
		if (pan_drive->receive_state >= 9) {
			pan_drive->receive_state = 0;
		}
	}
}

static void device_PANdrive_new_control_message(void* const _pan_drive,
		unsigned int count, const control_parameter_t* parameters) {
	device_PANdrive_t* pan_drive = (device_PANdrive_t*) _pan_drive;

	pan_drive_command_code_cpt command = 0;
	unsigned char type = 0;
	unsigned char mbnum = 0;
	long value = 0;

	while (count--) {
		switch (parameters->type) {
		case PAN_DRIVE_COMMAND:
			command = parameters->value;
			break;
		case PAN_DRIVE_TYPE:
			type = parameters->value;
			break;
		case PAN_DRIVE_MOTOR_NUMBER:
			mbnum = parameters->value;
			break;
		case PAN_DRIVE_BANK_NUMBER:
			mbnum = parameters->value;
			break;
		case PAN_DRIVE_VALUE:
			value = parameters->value;
			break;
		case PAN_DRIVE_MODULE_ADDRESS:
			pan_drive->target_address = parameters->value;
			break;
		default:  //interpret "unknown" parameters as direct command codes
			command = parameters->type;
			value = parameters->value;
			break;
		}
		parameters++;
	}
	device_PANdrive_send_command_w_mbnum(pan_drive, command, type, mbnum, value);
}

void device_PANdrive_send_command(device_PANdrive_t* const pan_drive,
		pan_drive_command_code_cpt command_code, pan_drive_command_type type,
		long value) {
	device_PANdrive_send_command_w_mbnum(pan_drive, command_code, type, 0, value);
}

void device_PANdrive_send_command_w_mbnum(device_PANdrive_t* const pan_drive,
		pan_drive_command_code_cpt command_code, unsigned char type,
		unsigned char mbnum, long value) {
	uart_light_regs_t* uart_light = pan_drive->uart_light;

	unsigned char checksum = pan_drive->target_address;
	uart_light_send(uart_light, pan_drive->target_address);
	checksum += command_code;
	uart_light_send(uart_light, command_code);
	checksum += type;
	uart_light_send(uart_light, type);
	checksum += mbnum;
	uart_light_send(uart_light, mbnum);

	int i = 4;
	unsigned char byte;
	while (i--) {
		byte = (value >> 24);
		value <<= 8;

		checksum += byte;
		uart_light_send(uart_light, byte);
	}
	uart_light_send(uart_light, checksum);

	//pan_drive->receive_state = 0;
	//TODO reset this at some point to react to transmission errors
	//but here is too early: last reply of PANdrive could still be in transmission
	//-> use some timeout, e.g. amount of calls of update function? or a real timer?
}

void device_PANdrive_set_target_address(device_PANdrive_t* const pan_drive,
		unsigned char new_address) {
	pan_drive->target_address = new_address;
}

void device_PANdrive_set_axis_parameter(device_PANdrive_t* const pan_drive,
		pan_drive_command_type type, long value) {
	device_PANdrive_send_command(pan_drive, PAN_DRIVE_COMMAND_SET_AXIS_PARAMETER,
			type, value);
}
