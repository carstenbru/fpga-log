/**
 * @file device_hct99.h
 * @brief HCT-99 optometer device driver
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_HCT99_H_
#define DEVICE_HCT99_H_

#include <uart.h>

#include "datastream_source.h"
#include "data_port.h"
#include "control_port.h"
#include "simple_float.h"

/**
 * @brief enumeration of the different control commands of the HCT-99
 */
typedef enum {
	//measure/output commands
	HCT99_COMMAND_MEASURE_SINGLE_CHANNEL = 'F', /**< start measurement of result (single channel) */
	HCT99_COMMAND_MEASURE_COLOR_VALUE = 'G', /**< start measurement/output of color value */
	HCT99_COMMAND_MEASURE_TRANSMISSION_REFLECTION = 'K', /**< start measurement/output transmission/reflection */
	HCT99_COMMAND_MEASURE_SPECIAL_CALCULATION = 'L', /**< start measurement/output special calculation */
	//measurement parameter commands
	HCT99_COMMAND_RANGE = 'H', /**< measurement range */
	HCT99_COMMAND_AUTORANGE = 'I', /**< autorange */
	HCT99_COMMAND_MEASUREMENT_TIME = 'M', /**< measurement (integration) time, could also be used for M2/M3 commands */
	HCT99_COMMAND_MEASUREMENT_TIME_SAVE = 'A', /**< save measurement time parameters (representation of M2 command) */
	HCT99_COMMAND_MEASUREMENT_TIME_LOAD = 'B', /**< load measurement time parameters (representation of M3 command) */
	HCT99_COMMAND_OFFSET = 'm', /**< offset */
	HCT99_COMMAND_MEASUREMENT_MODE = 'd', /**< measurement mode */
	HCT99_COMMAND_CAL_DATA_SELECT = 'i', /**< calibration data selection */
	HCT99_COMMAND_ZERO_ADJUST = 'J', /**< zero adjust */
	HCT99_COMMAND_SAVE_PARAMS_EEPROM = 'N', /**< save parameters in eeprom */
	HCT99_COMMAND_LOAD_PARAMS_EEPROM = 'O', /**< load parameters from eeprom */
	//miscellaneous commands
	HCT99_COMMAND_SPECIAL_DEV_CODE = 's', /**< special device code */
	HCT99_COMMAND_FIRMWARE_VERSION = 'P', /**< firmware version */
	HCT99_COMMAND_FIRMWARE_TYPE = 'Q', /**< firmware type */
	HCT99_COMMAND_HARDWARE_TYPE = 'S', /**< hardware type */
	HCT99_COMMAND_CAL_DATA_TEXT_INFO = 'n' /**< output of calibration data text info */
} hct99_command;

/**
 * @brief enumeration of the different error commands of the HCT-99, see HCT-99 manual for description
 */
typedef enum {
	HCT99_ERROR_CODE_OK = 0,
	HCT99_ERROR_CODE_OVERLOAD = 1,
	HCT99_ERROR_CODE_UNDERLOAD = 2,
	HCT99_ERROR_CODE_EEPROM = 4,
	HCT99_ERROR_CODE_BATTERY_LOW = 8,
	HCT99_ERROR_CODE_CHOPPER = 16,
	HCT99_ERROR_CODE_EXT_EEPROM = 32,
	HCT99_ERROR_CODE_EEPROM_WRITE = 64,
	HCT99_ERROR_CODE_CAL_DATA = 128,
	HCT99_ERROR_CODE_SUM_LOW = 256,
} hct99_error_code;

#define HCT99_NAMES_CHANNEL { "ch0", "ch1", "ch2", "ch3" }
#define HCT99_NAMES_TRANSMISSION_REFLECTION { "direct transmission", "diffuse transmission",\
	"sum transmission", "reflection" }
#define HCT99_NAMES_COLOR_1 { "x", "y", "Y", "u", "y", "Tc", "Xshort", "Xlong", "Z" }
#define HCT99_NAMES_COLOR_2 { "delta uv", "u'", "v'", "L*", "L99", "a*", "b*", "", "E*" }
#define HCT99_NAMES_COLOR_2_ALT { "a99", "b99", "", "E99" }
#define HCT99_NAMES_COLOR_3 { "Tristimulus X", "Tristimulus Y", "Tristimulus Z", "White adjustment" }

#define HCT99_ERROR_DEVICE "device error"
#define HCT99_ERROR_UNEXPECTED_DATA_RECEIVED "unexpected data"
#define HCT99_ERROR_PARAM_OUT_OF_RANGE "parameter out of range"
#define HCT99_ERROR_MISSING_COMMAND_CODE "missing command code"
#define HCT99_ERROR_COMMAND_FIFO_FULL "command fifo full"

#define HCT99_COMMAND_FIFO_SIZE 16

/**
 * @brief enumeration of next expected data from HCT-99 device
 */
typedef enum {
	HCT99_EXPECT_COEFFICIENT, /**< expecting coefficient */
	HCT99_EXPECT_EXPONENT_1, /**< expecting first digit of exponent */
	HCT99_EXPECT_EXPONENT_2, /**< expecting second digit of exponent */
	HCT99_EXPECT_ERR_CODE, /**< expecting error code */
	HCT99_EXPECT_NUMBER, /**< expecting a simple number (e.g. firmware version) */
	HCT99_EXPECT_LF, /**< expecting line feed */
	HCT99_EXPECT_NOTHING /**< expecting no data */
} hct99_expect_value;

/**
 * @brief struct describing a HCT-99 command
 */
typedef struct {
	hct99_command command_code; /**< hct-99 command code */
	unsigned int x; /**< x-parameter */
	unsigned int y; /**< x-parameter */
	unsigned int z; /**< x-parameter */
} hct99_command_t;

/**
 * @brief struct describing a HCT-99 device
 */
typedef struct {
	datastream_source_t super; /**< super-"class": datastream_source_t*/

	const data_port_t* data_out; /**< data output destination */
	const data_port_t* error_out; /**< error output destination */
	const data_port_t* misc_out; /**< miscellaneous commands data (firmware version,..) output destination */
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */

	uart_light_regs_t* uart_light; /**< pointer to UART hardware registers */

	hct99_expect_value expected_byte; /**< next expected data from HCT-99 device */
	simple_float_b10_t value; /**< currently parsed value */
	unsigned char val_neg; /**< indicates weather the currently parsed value is negative */
	unsigned char err_code; /**< currently parsed error code */
	const char* val_name; /**< current value name */
	const char* err_name; /**< current error name */
	unsigned char color_alternative; /**< current color mode of HCT-99 */

	hct99_command_t command_fifo[HCT99_COMMAND_FIFO_SIZE]; /**< command fifo to cache incoming commands till HCT99 is ready */
	unsigned int command_fifo_elements; /**< number of elements in fifo */
	unsigned int command_fifo_top; /**< number of top element in fifo */
} device_hct99_t;

/**
 * @brief HCT-99 init function
 *
 * Initializes the HCT-99 device, should be called before using the device.
 *
 * @param	hct99				pointer to the hct99 device
 * @param uart_light 	pointer to a uart_light peripheral
 * @param id					id of the new device (for log output, etc)
 */
void device_hct99_init(device_hct99_t* const hct99,
		uart_light_regs_t* const uart_light, const int id);

/**
 * @brief connects the data output port of a HCT-99 device to a given destination
 *
 * @param	hct99			pointer to the HCT-99 device
 * @param	data_in		the new data destination
 */
void device_hct99_set_data_out(device_hct99_t* const hct99,
		const data_port_t* const data_in);

/**
 * @brief connects the error output port of a HCT-99 device to a given destination
 *
 * The error output is also a data_port but outputs no measurement data but error codes.
 *
 * @param	hct99			pointer to the HCT-99 device
 * @param	data_in		the new data destination
 */
void device_hct99_set_error_out(device_hct99_t* const hct99,
		const data_port_t* const data_in);

/**
 * @brief connects the misc output port of a HCT-99 device to a given destination
 *
 * The miscellaneous output is also a data_port but outputs no measurement data but answers
 * to miscellaneous commands like firmware version, etc.
 *
 * @param	hct99			pointer to the HCT-99 device
 * @param	data_in		the new data destination
 */
void device_hct99_set_misc_out(device_hct99_t* const hct99,
		const data_port_t* const data_in);

/**
 * @brief executes (sends) a command on a HCT-99 device
 *
 * The device will be stored in a queue until the device is ready to execute it.
 *
 * @param hct99			pointer to the HCT-99 device
 * @param command		the command to execute
 */
void device_hct99_execute_command(device_hct99_t* const hct99,
		hct99_command_t command);

#endif
