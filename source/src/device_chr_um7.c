/**
 * @file device_chr_um7.c
 * @brief CHR-UM7 orientation sensor device driver (Binary protocol)
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_chr_um7.h>
#include <fpga-log/peripheral_funcs/uart_light_funcs.h>

const char* chr_um7_checksum_error = "checksum error";

/**
 * @brief CHR-UM7 device send data function
 *
 * @param	_chr_um7		pointer to the gps device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_chr_um7_send_data(void* const _chr_um7,
		const unsigned int id, const timestamp_t* const timestamp);

void device_chr_um7_init(device_chr_um7_t* const chr_um7,
		uart_light_regs_t* const uart_light, const int id) {
	datastream_source_init(&chr_um7->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	chr_um7->super.send_data = device_chr_um7_send_data;

	chr_um7->orientation_data_out = &data_port_dummy;
	chr_um7->proc_data_out = &data_port_dummy;
	chr_um7->raw_data_out = &data_port_dummy;
	chr_um7->error_out = &data_port_dummy;

	chr_um7->uart_light = uart_light;

	chr_um7->parse_state = 0;

	uart_light_enable_rxint(uart_light);
}

void device_chr_um7_set_orientation_data_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in) {
	chr_um7->orientation_data_out = data_in;
}

void device_chr_um7_set_proc_data_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in) {
	chr_um7->proc_data_out = data_in;
}

void device_chr_um7_set_raw_data_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in) {
	chr_um7->raw_data_out = data_in;
}

void device_chr_um7_set_error_out(device_chr_um7_t* const chr_um7,
		const data_port_t* const data_in) {
	chr_um7->error_out = data_in;
}

static void device_chr_um7_send_and_add_to_chksum(uart_light_regs_t* uart,
		unsigned int data, unsigned int* chksum) {
	uart_light_send(uart, data);
	*chksum += data;
}

static void device_chr_um7_send_0bytes(uart_light_regs_t* uart,
		unsigned int amount) {
	unsigned int i;
	for (i = 0; i < amount; i++)
		uart_light_send(uart, 0);
}

void device_chr_um7_set_rates(device_chr_um7_t* const chr_um7,
		uint_least8_t raw_accel_rate, uint_least8_t raw_gyro_rate,
		uint_least8_t raw_mag_rate, uint_least8_t temp_rate,
		uint_least8_t proc_accel_rate, uint_least8_t proc_gyro_rate,
		uint_least8_t proc_mag_rate, uint_least8_t quat_rate,
		uint_least8_t euler_rate, device_chr_um7_health_rate health_rate) {
	uart_light_regs_t* uart = chr_um7->uart_light;
	unsigned int chksum;

	device_chr_um7_send_and_add_to_chksum(uart, 's', &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, 'n', &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, 'p', &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, 0b11011100, &chksum);  //has_data, is_batch, bl3-0,hidden,command failed
	device_chr_um7_send_and_add_to_chksum(uart, 1, &chksum);  //address: 1

	device_chr_um7_send_and_add_to_chksum(uart, raw_accel_rate, &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, raw_gyro_rate, &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, raw_mag_rate, &chksum);
	uart_light_send(uart, 0);

	device_chr_um7_send_and_add_to_chksum(uart, temp_rate, &chksum);
	device_chr_um7_send_0bytes(uart, 3);

	device_chr_um7_send_and_add_to_chksum(uart, proc_accel_rate, &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, proc_gyro_rate, &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, proc_mag_rate, &chksum);
	device_chr_um7_send_0bytes(uart, 5);

	device_chr_um7_send_and_add_to_chksum(uart, quat_rate, &chksum);
	device_chr_um7_send_and_add_to_chksum(uart, euler_rate, &chksum);
	device_chr_um7_send_0bytes(uart, 3);

	device_chr_um7_send_and_add_to_chksum(uart, health_rate, &chksum);
	device_chr_um7_send_0bytes(uart, 6);

	uart_light_send(uart, (chksum >> 8) & 255);
	uart_light_send(uart, chksum & 255);
}

static void device_chr_um7_set_data_length(chr_um7_packet_t* packet) {
	if (packet->has_data) {
		if (packet->is_batch) {
			packet->data_length = packet->batch_length;
		} else {
			packet->data_length = 1;
		}
	} else {
		packet->data_length = 0;
	}
}

static void device_chr_um7_send_2x_16bit(device_chr_um7_t* const chr_um7,
		const data_port_t* data_out, const timestamp_t* const timestamp,
		char* first, char* second, uint8_t* data) {
	int tmp = data[1] | (data[0] << 8);
	if (data[0] & 128)  //sign extend
		tmp -= 65536;
	data_package_t package = { chr_um7->super.id, first, DATA_TYPE_INT, &tmp,
			timestamp };
	data_out->new_data(data_out->parent, &package);
	if (second) {
		tmp = data[3] | (data[2] << 8);
		if (data[2] & 128)  //sign extend
			tmp -= 65536;
		package.val_name = second;
		data_out->new_data(data_out->parent, &package);
	}
}

//TODO send real float, not just long integer!
static void device_chr_um7_send_float(device_chr_um7_t* const chr_um7,
		const data_port_t* data_out, const timestamp_t* const timestamp,
		char* val_name, uint8_t* data) {
	unsigned long tmp = data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];
	data_package_t package = { chr_um7->super.id, val_name, DATA_TYPE_LONG, &tmp,
			timestamp };
	data_out->new_data(data_out->parent, &package);
}

static void device_chr_um7_eval_health_reg(device_chr_um7_t* const chr_um7,
		const timestamp_t* const timestamp, uint8_t* data) {
	if (data[1] & 2) {
		data_package_t package = { chr_um7->super.id, "communication overflow",
				DATA_TYPE_BYTE, &data[1], timestamp };
		chr_um7->error_out->new_data(chr_um7->error_out->parent, &package);
	}
	if (data[0]) {
		data_package_t package = { chr_um7->super.id, "sensor error",
				DATA_TYPE_BYTE, &data[0], timestamp };
		chr_um7->error_out->new_data(chr_um7->error_out->parent, &package);
	}
}

static void device_chr_um7_process_received_register(
		device_chr_um7_t* const chr_um7, const timestamp_t* const timestamp,
		unsigned int address, uint8_t* data) {
	switch (address) {
	case 85:
		device_chr_um7_eval_health_reg(chr_um7, timestamp, data);
		break;
	case 86:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->raw_data_out, timestamp,
				"raw gyro x", "raw gyro y", data);
		break;
	case 87:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->raw_data_out, timestamp,
				"raw gyro z", 0, data);
		break;
	case 89:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->raw_data_out, timestamp,
				"raw acc x", "raw acc y", data);
		break;
	case 90:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->raw_data_out, timestamp,
				"raw acc z", 0, data);
		break;
	case 92:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->raw_data_out, timestamp,
				"raw mag x", "raw mag y", data);
		break;
	case 93:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->raw_data_out, timestamp,
				"raw mag z", 0, data);
		break;
	case 95:
		device_chr_um7_send_float(chr_um7, chr_um7->raw_data_out, timestamp,
				"temperature", data);
		break;

	case 97:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc gyro x", data);
		break;
	case 98:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc gyro y", data);
		break;
	case 99:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc gyro z", data);
		break;
	case 101:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc acc x", data);
		break;
	case 102:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc acc y", data);
		break;
	case 103:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc acc z", data);
		break;
	case 105:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc mag x", data);
		break;
	case 106:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc mag y", data);
		break;
	case 107:
		device_chr_um7_send_float(chr_um7, chr_um7->proc_data_out, timestamp,
				"proc mag z", data);
		break;

	case 109:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->orientation_data_out,
				timestamp, "quat a", "quat b", data);
		break;
	case 110:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->orientation_data_out,
				timestamp, "quat c", "quat d", data);
		break;

	case 112:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->orientation_data_out,
				timestamp, "roll", "pitch", data);
		break;
	case 113:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->orientation_data_out,
				timestamp, "yaw", 0, data);
		break;
	case 114:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->orientation_data_out,
				timestamp, "roll rate", "pitch rate", data);
		break;
	case 115:
		device_chr_um7_send_2x_16bit(chr_um7, chr_um7->orientation_data_out,
				timestamp, "yaw rate", 0, data);
		break;
	default:
		break;
	}
}

static void device_chr_um7_process_received_package(
		device_chr_um7_t* const chr_um7, const timestamp_t* const timestamp) {
	chr_um7_packet_t* packet = &chr_um7->um7_packet;
	int i;
	for (i = 0; i < packet->data_length; i++) {
		device_chr_um7_process_received_register(chr_um7, timestamp,
				packet->address + i, packet->data[i]);
	}
	if (packet->command_failed) {
		data_package_t package = { chr_um7->super.id, "command failed",
				DATA_TYPE_BYTE, &packet->address, timestamp };
		chr_um7->error_out->new_data(chr_um7->error_out->parent, &package);
	}
}

static void device_chr_um7_send_data(void* const _chr_um7,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_chr_um7_t* chr_um7 = (device_chr_um7_t*) _chr_um7;
	unsigned char byte;
	if (uart_light_receive_nb(chr_um7->uart_light, &byte) == UART_OK) {
		chr_um7->um7_packet.checksum += byte;
		if (chr_um7->parse_state < 3) {  //detect 's'+'n'+'p' (packet start)
			if (byte == 's') {
				chr_um7->parse_state = 1;
				chr_um7->um7_packet.checksum = 's';
			} else if (chr_um7->parse_state == 1) {
				chr_um7->parse_state = (byte == 'n') ? 2 : 0;
			} else if (chr_um7->parse_state == 2) {
				chr_um7->parse_state = (byte == 'p') ? 3 : 0;
			}
		} else {
			if (chr_um7->parse_state == 3) {  //package type
				chr_um7->um7_packet.pt = byte;
				device_chr_um7_set_data_length(&chr_um7->um7_packet);
			} else if (chr_um7->parse_state == 4) {  //address
				chr_um7->um7_packet.address = byte;
			} else if (chr_um7->parse_state
					<= 4 + (chr_um7->um7_packet.data_length * 4)) {  //data
				unsigned int i = (chr_um7->parse_state - 5) / 4;
				unsigned int b = (chr_um7->parse_state - 5) % 4;
				chr_um7->um7_packet.data[i][b] = byte;
			} else {  //checksum
				chr_um7->um7_packet.checksum -= byte;  //correct previously wrongly added checksum data
				if (chr_um7->parse_state & 1) {
					if (((chr_um7->um7_packet.checksum >> 8) & 255) != byte) {
						chr_um7->parse_state = -1;
						data_package_t package = { id, chr_um7_checksum_error,
								DATA_TYPE_BYTE, &byte, timestamp };
						chr_um7->error_out->new_data(chr_um7->error_out->parent, &package);
					}
				} else {
					if ((chr_um7->um7_packet.checksum & 255) != byte) {
						data_package_t package = { id, chr_um7_checksum_error,
								DATA_TYPE_BYTE, &byte, timestamp };
						chr_um7->error_out->new_data(chr_um7->error_out->parent, &package);
					} else {
						device_chr_um7_process_received_package(chr_um7, timestamp);
					}
					chr_um7->parse_state = -1;
				}
			}
			chr_um7->parse_state++;
		}
	}
}
