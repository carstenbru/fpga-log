#include <fpga-log/sink/control_protocol_openBCI.h>
#include <fpga-log/device/device_ads1299.h>

static void control_protocol_open_bci_parse(void* const _protocol,
		unsigned char new_byte);

void control_protocol_open_bci_init(
		control_protocol_open_bci_t* const control_protocol_ascii) {
	control_protocol_ascii->super.parse = control_protocol_open_bci_parse;
}

static void control_protocol_open_bci_parse(void* const _protocol,
		unsigned char new_byte) {
	control_protocol_open_bci_t* protocol =
			(control_protocol_open_bci_t*) _protocol;

	control_parameter_t parameter;
	switch (new_byte) {
	case 'q':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 1;
		break;
	case 'w':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 2;
		break;
	case 'e':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 3;
		break;
	case 'r':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 4;
		break;
	case 't':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 5;
		break;
	case 'y':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 6;
		break;
	case 'u':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 7;
		break;
	case 'i':
		parameter.type = ADS1299_COMMAND_ACTIVATE_CHANNEL;
		parameter.value = 8;
		break;
	case '1':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 1;
		break;
	case '2':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 2;
		break;
	case '3':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 3;
		break;
	case '4':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 4;
		break;
	case '5':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 5;
		break;
	case '6':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 6;
		break;
	case '7':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 7;
		break;
	case '8':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_CHANNEL;
		parameter.value = 8;
		break;

	case '!':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 1;
		break;
	case '@':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 2;
		break;
	case '#':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 3;
		break;
	case '$':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 4;
		break;
	case '%':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 5;
		break;
	case '^':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 6;
		break;
	case '&':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 7;
		break;
	case '*':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN;
		parameter.value = 8;
		break;

	case 'Q':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 1;
		break;
	case 'W':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 2;
		break;
	case 'E':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 3;
		break;
	case 'R':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 4;
		break;
	case 'T':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 5;
		break;
	case 'Y':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 6;
		break;
	case 'U':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 7;
		break;
	case 'I':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN;
		parameter.value = 8;
		break;

	case 'A':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 1;
		break;
	case 'S':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 2;
		break;
	case 'D':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 3;
		break;
	case 'F':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 4;
		break;
	case 'G':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 5;
		break;
	case 'H':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 6;
		break;
	case 'J':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 7;
		break;
	case 'K':
		parameter.type = ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN;
		parameter.value = 8;
		break;

	case 'Z':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 1;
		break;
	case 'X':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 2;
		break;
	case 'C':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 3;
		break;
	case 'V':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 4;
		break;
	case 'B':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 5;
		break;
	case 'N':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 6;
		break;
	case 'M':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 7;
		break;
	case '<':
		parameter.type = ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN;
		parameter.value = 8;
		break;

	default:
		parameter.type = new_byte;
		parameter.value = 0;
		break;
	}
	protocol->super.send_message(protocol->super.param, 0, 1, &parameter);
}
