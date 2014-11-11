#include <fpga-log/device/device_ads1299.h>
#include <spi.h>
#include <fpga-log/peripheral_funcs/spi_funcs.h>

//TODO remove delays with this functions!
void sleep(unsigned int i) {
	while (i > 0) {
		__asm__("");
		i--;
	}
}

static void device_ads1299_send_data(void* const _ads1299,
		const unsigned int id, const timestamp_t* const timestamp);
static void device_ads1299_update(void* const _ads1299);
static void device_ads1299_send_command(device_ads1299_t* const ads1299,
		ads1299_command command);
static int device_ads1299_read_reg(device_ads1299_t* const ads1299, int reg);
static void device_ads1299_write_reg(device_ads1299_t* const ads1299, int reg,
		int data);
static void device_ads1299_read_conv_res(device_ads1299_t* const ads1299,
		char* buf);
static void device_ads1299_new_control_message(void* const _ads1299,
		unsigned int count, const control_parameter_t* parameters);
static void device_ads1299_alter_bias_based_on_channel_state(
		device_ads1299_t* const ads1299, int channel);
static int device_ads1299_is_channel_active(device_ads1299_t* ads1299,
		int channel);
static void device_ads1299_activate_channel_bias(device_ads1299_t* ads1299,
		int channel);
static void device_ads1299_deactivate_channel_bias(device_ads1299_t* ads1299,
		int channel);

void device_ads1299_init(device_ads1299_t* const ads1299,
		spi_master_regs_t* const spi_master, const int drdy_id, ads1299_srb2 srb2,
		ads1299_data_rate sample_rate) {
	datastream_source_init(&ads1299->super, drdy_id);

	ads1299->super.super.update = device_ads1299_update;
	ads1299->super.send_data = device_ads1299_send_data;

	ads1299->spi_master = spi_master;

	ads1299->data_out = &data_port_dummy;
	ads1299->control_in = control_port_dummy;
	ads1299->control_in.parent = (void*) ads1299;
	ads1299->control_in.new_control_message = device_ads1299_new_control_message;

	ads1299->running = 0;
	ads1299->use_srb2 = srb2;
	ads1299->auto_bias = ADS1299_AUTO_BIAS_ON;

	spi_set_div((spi_t *) spi_master, 2);
	//CPOL = 0 is default
	spi_set_cpah((spi_t *) spi_master, 1);
	spi_enable((spi_t *) spi_master);
	sleep(10000);

	device_ads1299_send_command(ads1299, ADS1299_COMMAND_RESET);
	device_ads1299_send_command(ads1299, ADS1299_COMMAND_SDATAC);

	device_ads1299_write_reg(ads1299, ADS1299_REG_CONFIG1,
			0b10010000 | sample_rate);

	if (srb2 == ADS1299_SRB2_OPEN) {
		//we're using positive.  Set to default polarity
		device_ads1299_write_reg(ads1299, ADS1299_REG_LOFF_FLIP, 0b00000000);
	} else {
		//we're using negative.  flip the polarity
		device_ads1299_write_reg(ads1299, ADS1299_REG_LOFF_FLIP, 0b11111111);
	}

	int i;
	for (i = 1; i <= ADS1299_CHANNELS; i++) {  //really necessary to first turn all channels ofg??
		device_ads1299_configure_channel(ads1299, i, ADS1299_CHANNEL_POWER_DOWN,
				ADS1299_PGA_24, ADS1299_MUX_NORMAL, srb2);  //turn off the channel
		device_ads1299_change_channel_loff(ads1299, i, ADS1299_LOFF_NCHAN_DEACTIVATE);
		device_ads1299_change_channel_loff(ads1299, i, ADS1299_LOFF_PCHAN_DEACTIVATE);  //turn off any impedance monitoring
	}

	//srb1 setting
	if (srb2 == ADS1299_SRB2_OPEN) {
		device_ads1299_write_reg(ads1299, ADS1299_REG_BIAS_MISC1, 0b00100000);
	}

	//activate auto bias
	device_ads1299_set_auto_bias_generation(ads1299, ADS1299_AUTO_BIAS_ON);
	//configure internal test signal
	device_ads1299_config_test_signal(ads1299, ADS1299_TEST_SIG_AMP_1X,
			ADS1299_TEST_SIG_FREQ_FAST);

	device_ads1299_config_loff(ads1299, ADS1299_LOFF_FREQ_31_2HZ,
			ADS1299_LOFF_MAG_6NA, ADS1299_LOFF_THRES_95);

	//acitvate and configure all channels
	device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
			ADS1299_PGA_24, ADS1299_MUX_NORMAL, srb2);
}

void device_ads1299_set_data_out(device_ads1299_t* const ads1299,
		const data_port_t* const data_in) {
	ads1299->data_out = data_in;
}

control_port_t* device_ads1299_get_control_in(device_ads1299_t* const ads1299) {
	return &ads1299->control_in;
}

static void device_ads1299_change_running_state(device_ads1299_t* const ads1299,
		int state) {
	if (state) {
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_START);
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_RDATAC);
		ads1299->running = 1;
	} else {
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_SDATAC);
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_STOP);
		ads1299->running = 0;
	}
}

void device_ads1299_configure_channel(device_ads1299_t* const ads1299,
		int channel, ads1299_ach_active active, ads1299_pga pga, ads1299_mux mux,
		ads1299_srb2 srb2) {
	if (channel < 1 || channel > 8)
		return;

	//configure channel parameters
	device_ads1299_send_command(ads1299, ADS1299_COMMAND_SDATAC);
	device_ads1299_send_command(ads1299, ADS1299_COMMAND_STOP);
	device_ads1299_write_reg(ads1299, 4 + channel, active | pga | mux | srb2);

	//bias generation
	device_ads1299_alter_bias_based_on_channel_state(ads1299, channel);

	//srb1 setting
	if (srb2 == ADS1299_SRB2_OPEN) {
		device_ads1299_write_reg(ads1299, ADS1299_REG_BIAS_MISC1, 0b00100000);
	}

	//write config3 register (activate bias)
	device_ads1299_write_reg(ads1299, 3, 0b11101100);

	if (ads1299->running) {  //start sampling again if it was active..
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_START);
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_RDATAC);
	}
}

static void device_ads1299_new_control_message(void* const _ads1299,
		unsigned int count, const control_parameter_t* parameters) {
	device_ads1299_t* ads1299 = (device_ads1299_t*) _ads1299;

	int value = parameters->value;
	while (count--) {
		switch (parameters->type) {
		case ADS1299_COMMAND_START_RUNNING:
			device_ads1299_change_running_state(ads1299, 1);
			break;
		case ADS1299_COMMAND_STOP_RUNNING:
			device_ads1299_change_running_state(ads1299, 0);
			break;
		case ADS1299_COMMAND_ACTIVATE_CHANNEL:
			device_ads1299_configure_channel(ads1299, value, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_NORMAL, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_DEACTIVATE_CHANNEL:
			device_ads1299_configure_channel(ads1299, value,
					ADS1299_CHANNEL_POWER_DOWN, ADS1299_PGA_24, ADS1299_MUX_NORMAL,
					ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_AUTO_BIAS_ON:
			device_ads1299_set_auto_bias_generation(ads1299, ADS1299_AUTO_BIAS_ON);
			break;
		case ADS1299_COMMAND_AUTO_BIAS_OFF:
			device_ads1299_set_auto_bias_generation(ads1299, ADS1299_AUTO_BIAS_OFF);
			break;
		case ADS1299_COMMAND_TEST_SHORTED:
			device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_SHORT, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_TEST_SIG_1X_SLOW:
			device_ads1299_config_test_signal(ads1299, ADS1299_TEST_SIG_AMP_1X,
					ADS1299_TEST_SIG_FREQ_SLOW);
			device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_TEST, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_TEST_SIG_1X_FAST:
			device_ads1299_config_test_signal(ads1299, ADS1299_TEST_SIG_AMP_1X,
					ADS1299_TEST_SIG_FREQ_FAST);
			device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_TEST, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_TEST_SIG_1X_FAST2:
			device_ads1299_config_test_signal(ads1299, ADS1299_TEST_SIG_AMP_1X,
					ADS1299_TEST_SIG_FREQ_FAST);
			device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_TEST, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_TEST_SIG_2X_DC:
			device_ads1299_config_test_signal(ads1299, ADS1299_TEST_SIG_AMP_2X,
					ADS1299_TEST_SIG_FREQ_DC);
			device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_TEST, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_TEST_SIG_2X_SLOW:
			device_ads1299_config_test_signal(ads1299, ADS1299_TEST_SIG_AMP_2X,
					ADS1299_TEST_SIG_FREQ_SLOW);
			device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_TEST, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_TEST_SIG_2X_FAST:
			device_ads1299_config_test_signal(ads1299, ADS1299_TEST_SIG_AMP_2X,
					ADS1299_TEST_SIG_FREQ_FAST);
			device_ads1299_configure_all_channels(ads1299, ADS1299_CHANNEL_ACTIVE,
					ADS1299_PGA_24, ADS1299_MUX_TEST, ads1299->use_srb2);
			break;
		case ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN:
			device_ads1299_change_channel_loff(ads1299, value,
					ADS1299_LOFF_NCHAN_ACTIVATE);
			break;
		case ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN:
			device_ads1299_change_channel_loff(ads1299, value,
					ADS1299_LOFF_PCHAN_ACTIVATE);
			break;
		case ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN:
			device_ads1299_change_channel_loff(ads1299, value,
					ADS1299_LOFF_NCHAN_DEACTIVATE);
			break;
		case ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN:
			device_ads1299_change_channel_loff(ads1299, value,
					ADS1299_LOFF_PCHAN_DEACTIVATE);
			break;
		default:
			break;
		}
	}
}

static void device_ads1299_update(void* const _ads1299) {
}

static void device_ads1299_send_data(void* const _ads1299,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_ads1299_t* ads1299 = (device_ads1299_t*) _ads1299;

	char buf[9 * 3];  //(status register + 8 channels) * 3 byte
	device_ads1299_read_conv_res(ads1299, buf);

	data_package_t package = { id, ADS1299_DATA_NAME, DATA_TYPE_OPEN_BCI, buf,
			timestamp };
	ads1299->data_out->new_data(ads1299->data_out->parent, &package);
}

static void device_ads1299_send_command(device_ads1299_t* const ads1299,
		ads1299_command command) {
	spi_master_regs_t* const spi_master = ads1299->spi_master;

	spi_activate((spi_t *) spi_master, 1);
	sleep(10);
	spi_write(spi_master, command);
	while (!(spi_trans_finished(spi_master)))
		;
	sleep(10);
	spi_deactivate((spi_t *) spi_master);
	sleep(75);
}

static int device_ads1299_read_reg(device_ads1299_t* const ads1299, int reg) {
	spi_master_regs_t* const spi_master = ads1299->spi_master;

	spi_activate((spi_t *) spi_master, 1);
	sleep(10);
	spi_write(spi_master, 0b00100000 | reg);  //rreg command and reg address
	while (!(spi_trans_finished(spi_master)))
		;
	spi_write(spi_master, 0);  //read one register
	while (!(spi_trans_finished(spi_master)))
		;
	spi_write(spi_master, 0);
	while (!(spi_trans_finished(spi_master)))
		;
	sleep(10);
	spi_deactivate((spi_t *) spi_master);

	return spi_read_data_in(spi_master);
}

static void device_ads1299_write_reg(device_ads1299_t* const ads1299, int reg,
		int data) {
	spi_master_regs_t* const spi_master = ads1299->spi_master;

	spi_activate((spi_t *) spi_master, 1);
	sleep(10);
	spi_write(spi_master, 0b01000000 | reg);  //wreg command and reg address
	while (!(spi_trans_finished(spi_master)))
		;
	spi_write(spi_master, 0);  //write one register
	while (!(spi_trans_finished(spi_master)))
		;
	spi_write(spi_master, data);
	while (!(spi_trans_finished(spi_master)))
		;
	sleep(10);
	spi_deactivate((spi_t *) spi_master);
}

static void device_ads1299_read_conv_res(device_ads1299_t* const ads1299,
		char* buf) {
	spi_master_regs_t* const spi_master = ads1299->spi_master;

	spi_activate((spi_t *) spi_master, 1);
	sleep(10);

	int i;
	for (i = 0; i < 9 * 3; i++) {  //(status register + 8 channels) * 3 byte
		spi_write(spi_master, 0);
		while (!(spi_trans_finished(spi_master)))
			;
		*buf = spi_read_data_in(spi_master);
		buf++;
	}

	sleep(10);
	spi_deactivate((spi_t *) spi_master);
}

static int device_ads1299_is_channel_active(device_ads1299_t* ads1299,
		int channel) {
	return !(device_ads1299_read_reg(ads1299, 4 + channel) & 0b10000000);
}

static void device_ads1299_activate_channel_bias(device_ads1299_t* ads1299,
		int channel) {
	int reg = device_ads1299_read_reg(ads1299, ADS1299_REG_BIAS_SENSP);
	reg |= (1 << (channel - 1));
	device_ads1299_write_reg(ads1299, ADS1299_REG_BIAS_SENSP, reg);

	if (ads1299->use_srb2 == ADS1299_SRB2_CLOSED) {
		reg = device_ads1299_read_reg(ads1299, ADS1299_REG_BIAS_SENSN);
		reg |= (1 << (channel - 1));
		device_ads1299_write_reg(ads1299, ADS1299_REG_BIAS_SENSN, reg);
	}
}
static void device_ads1299_deactivate_channel_bias(device_ads1299_t* ads1299,
		int channel) {
	int reg = device_ads1299_read_reg(ads1299, ADS1299_REG_BIAS_SENSP);
	reg &= ~(1 << (channel - 1));
	device_ads1299_write_reg(ads1299, ADS1299_REG_BIAS_SENSP, reg);

	reg = device_ads1299_read_reg(ads1299, ADS1299_REG_BIAS_SENSN);
	reg &= ~(1 << (channel - 1));
	device_ads1299_write_reg(ads1299, ADS1299_REG_BIAS_SENSN, reg);
}

static void device_ads1299_alter_bias_based_on_channel_state(
		device_ads1299_t* const ads1299, int channel) {
	if ((ads1299->auto_bias == ADS1299_AUTO_BIAS_ON)
			&& device_ads1299_is_channel_active(ads1299, channel)) {
		device_ads1299_activate_channel_bias(ads1299, channel);
	} else {
		device_ads1299_deactivate_channel_bias(ads1299, channel);
	}
}

void device_ads1299_set_auto_bias_generation(device_ads1299_t* const ads1299,
		ads1299_auto_bias state) {
	ads1299->auto_bias = state;

	int i;
	for (i = 1; i <= ADS1299_CHANNELS; i++) {
		device_ads1299_alter_bias_based_on_channel_state(ads1299, i);
	}
}

void device_ads1299_config_test_signal(device_ads1299_t* const ads1299,
		ads1299_test_sig_amp amp, ads1299_test_sig_freq freq) {
	device_ads1299_write_reg(ads1299, ADS1299_REG_CONFIG2,
			0b11010000 | amp | freq);
}

void device_ads1299_configure_all_channels(device_ads1299_t* const ads1299,
		ads1299_ach_active active, ads1299_pga pga, ads1299_mux mux,
		ads1299_srb2 srb2) {
	int i;
	for (i = 1; i <= ADS1299_CHANNELS; i++) {
		device_ads1299_configure_channel(ads1299, i, active, pga, mux, srb2);
	}
}

void device_ads1299_config_loff(device_ads1299_t* const ads1299,
		ads1299_loff_freq frequency, ads1299_loff_mag magnitude,
		ads1299_loff_thres threshold) {
	device_ads1299_write_reg(ads1299, ADS1299_REG_LOFF,
			frequency | magnitude | threshold);
}

void device_ads1299_change_channel_loff(device_ads1299_t* const ads1299,
		int channel, ads1299_loff_config setting) {
	if (channel < 1 || channel > 8)
		return;
	device_ads1299_send_command(ads1299, ADS1299_COMMAND_SDATAC);
	device_ads1299_send_command(ads1299, ADS1299_COMMAND_STOP);

	int reg = ADS1299_REG_LOFF_SENSP;
	if (setting == ADS1299_LOFF_NCHAN_ACTIVATE
			|| setting == ADS1299_LOFF_NCHAN_DEACTIVATE)
		reg = ADS1299_REG_LOFF_SENSN;

	int val = device_ads1299_read_reg(ads1299, reg);

	if (setting == ADS1299_LOFF_NCHAN_ACTIVATE
			|| setting == ADS1299_LOFF_PCHAN_ACTIVATE) {
		val |= (1 << (channel - 1));
	} else {
		val &= ~(1 << (channel - 1));
	}
	device_ads1299_write_reg(ads1299, reg, val);

	if (ads1299->running) {  //start sampling again if it was active..
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_START);
		device_ads1299_send_command(ads1299, ADS1299_COMMAND_RDATAC);
	}
}
