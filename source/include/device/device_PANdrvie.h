/**
 * @file device_PANdrive.h
 * @brief device driver (UART) for PANdrive motors of trinamic
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DEVICE_PANDRIVE_H_
#define DEVICE_PANDRIVE_H_

#include <uart.h>

#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>
#include <fpga-log/control_port.h>

#define PAN_DRIVE_DEFAULT_TARGET_ADDRESS 0x01

#define PAN_DRIVE_ERROR_CODES { "wrong checksum", "invalid command", "wrong type", "invalid value", "EEPROM locked", "not available" }

typedef enum {
	PAN_DRIVE_MODULE_ADDRESS = 'a', /** set the address of the module to talk to */
	PAN_DRIVE_COMMAND = 'c', /** @value_ref pan_drive_command_code_cpt issue a command */
	PAN_DRIVE_TYPE = 't', /** type field - not needed for most instructions */
	PAN_DRIVE_MOTOR_NUMBER = 'm', /** number of the motor to access */
	PAN_DRIVE_BANK_NUMBER = 'b', /** bank of the motor to access */
	PAN_DRIVE_VALUE = 'v', /** value for the command */
} pan_drive_command_cpt;

typedef enum {
	PAN_DRIVE_COMMAND_ROTATE_RIGHT = 1, /** rotate right with a specified velocity */
	PAN_DRIVE_COMMAND_ROTATE_LEFT = 2, /** rotate left with a specified velocity */
	PAN_DRIVE_COMMAND_MOTOR_STOP = 3, /** @value_ref none stop the motor */
	PAN_DRIVE_COMMAND_MOVE_TO_POSTION = 4, /** move to a specified position */

	PAN_DRIVE_COMMAND_SET_AXIS_PARAMETER = 5, /** set an axis parameter - refer to datasheet for parameters */
	PAN_DRIVE_COMMAND_GET_AXIS_PARAMETER = 6, /** @value_ref none get an axis parameter - refer to datasheet for parameters */
	PAN_DRIVE_COMMAND_STORE_AXIS_PARAMTER = 7, /** @value_ref none store an axis parameter - refer to datasheet for parameters */
	PAN_DRIVE_COMMAND_RESTORE_AXIS_PARAMTER = 8, /** @value_ref none restore an axis parameter - refer to datasheet for parameters */

	PAN_DRIVE_COMMAND_SET_GLOBAL_PARAMETER = 9, /** set an global parameter - refer to datasheet for parameters */
	PAN_DRIVE_COMMAND_GET_GLOBAL_PARAMETER = 10, /** @value_ref none get an global parameter - refer to datasheet for parameters */
	PAN_DRIVE_COMMAND_STORE_GLOBAL_PARAMTER = 11,  /** @value_ref none store an global parameter - refer to datasheet for parameters */
	PAN_DRIVE_COMMAND_RESTORE_GLOBAL_PARAMTER = 12, /** @value_ref none restore an global parameter - refer to datasheet for parameters */

	PAN_DRIVE_COMMAND_REFERENCE_SEARCH = 13, /** @value_ref none search for reference */
	PAN_DRIVE_COMMAND_SET_OUTPUT = 14, /** set general digital outputs */
	PAN_DRIVE_COMMAND_GET_IN_OUTPUT = 15, /** @value_ref none get general digital in/outputs */

	/* gap: TMCL only commands */

	PAN_DRIVE_COMMAND_SET_COORDINATE = 30, /** set a coordinate */
	PAN_DRIVE_COMMAND_GET_COORDINATE = 31, /** @value_ref none get a coordinate */
	PAN_DRIVE_COMMAND_CAPTURE_COORDINATE = 32, /** @value_ref none capture a coordinate */

	/* TODO implement accumulator commands? not really necessary... */

	PAN_DRIVE_COMMAND_TARGET_REACHED_EVENT = 138, /** @value_ref none send an extra event when target reached */
} pan_drive_command_code_cpt;

typedef enum {
	PAN_DRIVE_TYPE_DONT_CARE = 0,

	/* types for PAN_DRIVE_COMMAND_MOVE_TO_POSTION */
	PAN_DRIVE_TYPE_POS_ABS = 0,
	PAN_DRIVE_TYPE_POS_REL = 1,
	PAN_DRIVE_TYPE_POS_COORD = 2,

	PAN_DRIVE_TYPE_APARAM_TARGET_POS = 0,
	PAN_DRIVE_TYPE_APARAM_ACT_POS = 1,
	PAN_DRIVE_TYPE_APARAM_TARGET_SPEED = 2,
	PAN_DRIVE_TYPE_APARAM_ACT_SPEED = 3,
	PAN_DRIVE_TYPE_APARAM_MAX_POS_SPEED = 4,
	PAN_DRIVE_TYPE_APARAM_MAX_ACC = 5,
	PAN_DRIVE_TYPE_APARAM_MAX_CUR = 6,
	PAN_DRIVE_TYPE_APARAM_STDBY = 7,
	PAN_DRIVE_TYPE_APARAM_TARGET_REACHED = 8,
	PAN_DRIVE_TYPE_APARAM_REF_SW = 9,
	PAN_DRIVE_TYPE_APARAM_RIGHT_SW = 10,
	PAN_DRIVE_TYPE_APARAM_LEFT_SW = 11,
	PAN_DRIVE_TYPE_APARAM_RIGHT_SW_DISABLE = 12,
	PAN_DRIVE_TYPE_APARAM_LEFT_SW_DISABLE = 13,
	PAN_DRIVE_TYPE_APARAM_MIN_SPEED = 130,
	PAN_DRIVE_TYPE_APARAM_ACT_ACC = 135,
	PAN_DRIVE_TYPE_APARAM_RAMP_MODE = 138,
	PAN_DRIVE_TYPE_APARAM_MICROSTEP_RES = 140,
	PAN_DRIVE_TYPE_APARAM_SOFT_STOP = 149,
	PAN_DRIVE_TYPE_APARAM_RAMP_DIV = 153,
	PAN_DRIVE_TYPE_APARAM_PULSE_DIV = 154,
	PAN_DRIVE_TYPE_APARAM_REF_MODE = 193,
	PAN_DRIVE_TYPE_APARAM_REF_SEARCH_SPEED = 194,
	PAN_DRIVE_TYPE_APARAM_REF_SW_SPEED = 195,
	PAN_DRIVE_TYPE_APARAM_DIST_SW = 196,
	PAN_DRIVE_TYPE_APARAM_MIXED_DECAY = 203,
	PAN_DRIVE_TYPE_APARAM_FREEWHEELING = 204,
	PAN_DRIVE_TYPE_APARAM_STALL_THRESHOLD = 205,
	PAN_DRIVE_TYPE_APARAM_ACT_LOAD = 206,
	PAN_DRIVE_TYPE_APARAM_DRIVER_ERROR = 208,
	PAN_DRIVE_TYPE_APARAM_ENC_POS = 210,
	PAN_DRIVE_TYPE_APARAM_FULLSTEP_THRESHOLD = 211,
	PAN_DRIVE_TYPE_APARAM_MAX_ENC_DERIVATION = 212,
	PAN_DRIVE_TYPE_APARAM_POWER_DOWN_DELAY = 214,
	PAN_DRIVE_TYPE_APARAM_ABS_ENC_VAL = 215,
	/* TODO Global parameters (chapter 7) not so important.. */

	/* types for PAN_DRIVE_COMMAND_REFERENCE_SEARCH */
	PAN_DRIVE_TYPE_REF_START = 0,
	PAN_DRIVE_TYPE_REF_STOP = 1,
	PAN_DRIVE_TYPE_REF_STATUS = 2,

	/* types for PAN_DRIVE_COMMAND_SET_OUTPUT / PAN_DRIVE_COMMAND_GET_IN_OUTPUT */
	PAN_DRIVE_TYPE_PIN_0 = 0,
	PAN_DRIVE_TYPE_PIN_1 = 1,

	/* types for PAN_DRIVE_COMMAND_SET_COORDINATE / PAN_DRIVE_COMMAND_GET_COORDINATE / PAN_DRIVE_COMMAND_CAPTURE_COORDINATE */
	PAN_DRIVE_TYPE_COORDINATE_0 = 0,
	PAN_DRIVE_TYPE_COORDINATE_1 = 1,
	PAN_DRIVE_TYPE_COORDINATE_2 = 2,
	PAN_DRIVE_TYPE_COORDINATE_3 = 3,
	PAN_DRIVE_TYPE_COORDINATE_4 = 4,
	PAN_DRIVE_TYPE_COORDINATE_5 = 5,
	PAN_DRIVE_TYPE_COORDINATE_6 = 6,
	PAN_DRIVE_TYPE_COORDINATE_7 = 7,
	PAN_DRIVE_TYPE_COORDINATE_8 = 8,
	PAN_DRIVE_TYPE_COORDINATE_9 = 9,
	PAN_DRIVE_TYPE_COORDINATE_10 = 10,
	PAN_DRIVE_TYPE_COORDINATE_11 = 11,
	PAN_DRIVE_TYPE_COORDINATE_12 = 12,
	PAN_DRIVE_TYPE_COORDINATE_13 = 13,
	PAN_DRIVE_TYPE_COORDINATE_14 = 14,
	PAN_DRIVE_TYPE_COORDINATE_15 = 15,
	PAN_DRIVE_TYPE_COORDINATE_16 = 16,
	PAN_DRIVE_TYPE_COORDINATE_17 = 17,
	PAN_DRIVE_TYPE_COORDINATE_18 = 18,
	PAN_DRIVE_TYPE_COORDINATE_19 = 19,
	PAN_DRIVE_TYPE_COORDINATE_20 = 20,
} pan_drive_command_type;

/**
 * @brief struct describing a PANdrive device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* data_out; /**< data output destination */
	const data_port_t* status_out; /**< status output destination */
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */

	uart_light_regs_t* uart_light; /**< pointer to UART hardware registers */

	unsigned char target_address;
	int receive_state;
	unsigned char received_status;
	int reply_command_code;
	long received_value;

	char* buf;
} device_PANdrive_t;

void device_PANdrive_init(device_PANdrive_t* const pan_drive,
		uart_light_regs_t* const uart_light, const int id);

control_port_t* device_PANdrive_get_control_in(
		device_PANdrive_t* const pan_drive);

void device_PANdrive_set_data_out(device_PANdrive_t* const pan_drive,
		const data_port_t* const data_in);

void device_PANdrive_set_status_out(device_PANdrive_t* const pan_drive,
		const data_port_t* const data_in);

void device_PANdrive_send_command(device_PANdrive_t* const pan_drive,
		pan_drive_command_code_cpt command_code, pan_drive_command_type type,
		long value);

void device_PANdrive_send_command_w_mbnum(device_PANdrive_t* const pan_drive,
		pan_drive_command_code_cpt command_code, unsigned char type,
		unsigned char mbnum, long value);

void device_PANdrive_set_target_address(device_PANdrive_t* const pan_drive,
		unsigned char new_address);

void device_PANdrive_set_axis_parameter(device_PANdrive_t* const pan_drive,
		pan_drive_command_type type, long value);

#endif
