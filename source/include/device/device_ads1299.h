#ifndef DEVICE_ADS1299_H_
#define DEVICE_ADS1299_H_

#include <fpga-log/datastream_source.h>

#include <fpga-log/data_port.h>
#include <fpga-log/control_port.h>
#include <peripherals/spi_master.h>

#define ADS1299_DATA_NAME "EEG-Data"
#define DATA_TYPE_OPEN_BCI 42
#define ADS1299_CHANNELS 8

typedef enum {
	ADS1299_AUTO_BIAS_OFF = 0, ADS1299_AUTO_BIAS_ON = 1
} ads1299_auto_bias;

typedef enum {
	ADS1299_CHANNEL_ACTIVE = 0b00000000, ADS1299_CHANNEL_POWER_DOWN = 0b10000000
} ads1299_ach_active;

typedef enum {
	ADS1299_SRB2_CLOSED = 0b00001000, ADS1299_SRB2_OPEN = 0b00000000
} ads1299_srb2;

typedef enum {
	ADS1299_LOFF_NCHAN_ACTIVATE,
	ADS1299_LOFF_NCHAN_DEACTIVATE,
	ADS1299_LOFF_PCHAN_ACTIVATE,
	ADS1299_LOFF_PCHAN_DEACTIVATE
} ads1299_loff_config;

typedef enum {
	ADS1299_PGA_1 = 0b00000000,
	ADS1299_PGA_2 = 0b00010000,
	ADS1299_PGA_4 = 0b00100000,
	ADS1299_PGA_8 = 0b00110000,
	ADS1299_PGA_12 = 0b01000000,
	ADS1299_PGA_24 = 0b01100000
} ads1299_pga;

typedef enum {
	ADS1299_MUX_NORMAL = 0b00000000,
	ADS1299_MUX_SHORT = 0b00000001,
	ADS1299_MUX_SPECIAL = 0b00000010,
	ADS1299_MUX_MVDD = 0b00000011,
	ADS1299_MUX_TEMP = 0b00000100,
	ADS1299_MUX_TEST = 0b00000101,
	ADS1299_MUX_BIAS_DRP = 0b00000110,
	ADS1299_MUX_BIAS_DRN = 0b00000111
} ads1299_mux;

typedef enum {
	ADS1299_TEST_SIG_AMP_1X = 0b00000000, ADS1299_TEST_SIG_AMP_2X = 0b00000100
} ads1299_test_sig_amp;

typedef enum {
	ADS1299_TEST_SIG_FREQ_SLOW = 0b00000000,
	ADS1299_TEST_SIG_FREQ_FAST = 0b00000001,
	ADS1299_TEST_SIG_FREQ_DC = 0b00000011
} ads1299_test_sig_freq;

typedef enum {
	ADS1299_DATA_RATE_250 = 0b0000110,
	ADS1299_DATA_RATE_500 = 0b0000101,
	ADS1299_DATA_RATE_1000 = 0b0000100,
	ADS1299_DATA_RATE_2000 = 0b0000011,
	ADS1299_DATA_RATE_4000 = 0b0000010,
	ADS1299_DATA_RATE_8000 = 0b0000001,
	ADS1299_DATA_RATE_16000 = 0b0000000
} ads1299_data_rate;

typedef enum {
	ADS1299_LOFF_FREQ_DC = 0b00000000,
	ADS1299_LOFF_FREQ_7_8HZ = 0b00000001,
	ADS1299_LOFF_FREQ_31_2HZ = 0b00000010,
	ADS1299_LOFF_FREQ_DR_4 = 0b00000011
} ads1299_loff_freq;

typedef enum {
	ADS1299_LOFF_MAG_6NA = 0b00000000,
	ADS1299_LOFF_MAG_24NA = 0b00000001,
	ADS1299_LOFF_MAG_6UA = 0b00000010,
	ADS1299_LOFF_MAG_24UA = 0b00000011
} ads1299_loff_mag;

typedef enum {
	ADS1299_LOFF_THRES_95 = 0b00000000,
	ADS1299_LOFF_THRES_92_5 = 0b00000001,
	ADS1299_LOFF_THRES_90 = 0b00000010,
	ADS1299_LOFF_THRES_87_5 = 0b00000011,
	ADS1299_LOFF_THRES_85 = 0b00000100,
	ADS1299_LOFF_THRES_80 = 0b00000101,
	ADS1299_LOFF_THRES_75 = 0b00000110,
	ADS1299_LOFF_THRES_70 = 0b00000111
} ads1299_loff_thres;

typedef enum {
	ADS1299_COMMAND_WAKEUP = 0x02,
	ADS1299_COMMAND_STANDBY = 0x04,
	ADS1299_COMMAND_RESET = 0x06,
	ADS1299_COMMAND_START = 0x08,
	ADS1299_COMMAND_STOP = 0x0A,
	ADS1299_COMMAND_RDATAC = 0x10,
	ADS1299_COMMAND_SDATAC = 0x11,
	ADS1299_COMMAND_RDATA = 0x12,
} ads1299_command;

typedef enum {
	ADS1299_COMMAND_ACTIVATE_CHANNEL = 'a', /**< activates a channel */
	ADS1299_COMMAND_DEACTIVATE_CHANNEL = 'd', /**< deactivates a channel */
	ADS1299_COMMAND_START_RUNNING = 'b', /**< @value_ref none starts the EEG */
	ADS1299_COMMAND_STOP_RUNNING = 's', /**< @value_ref none stops the EEG */
	ADS1299_COMMAND_AUTO_BIAS_ON = '`', /**< @value_ref none activates the auto-bias function */
	ADS1299_COMMAND_AUTO_BIAS_OFF = '~', /**< @value_ref none deactivates the auto-bias function */
	ADS1299_COMMAND_TEST_SHORTED = '0',  /**< @value_ref none activates the shorted test signal */
	ADS1299_COMMAND_TEST_SIG_1X_SLOW = '-', /**< @value_ref none activates the shorted test signal */
	ADS1299_COMMAND_TEST_SIG_1X_FAST = '+', /**< @value_ref none activates the 1x fast test signal */
	ADS1299_COMMAND_TEST_SIG_1X_FAST2 = '=', /**< @value_ref none activates the 1x fast2 test signal */
	ADS1299_COMMAND_TEST_SIG_2X_DC = 'p', /**< @value_ref none activates the 2x DC test signal */
	ADS1299_COMMAND_TEST_SIG_2X_SLOW = '[', /**< @value_ref none activates the 2x slow test signal */
	ADS1299_COMMAND_TEST_SIG_2X_FAST = ']', /**< @value_ref none activates the 2x fast test signal */

	ADS1299_COMMAND_ACTIVATE_LOFF_NCHAN = 'A', /**< activates the lead-off detection for a n-channel */
	ADS1299_COMMAND_ACTIVATE_LOFF_PCHAN = '!', /**< activates the lead-off detection for a p-channel */
	ADS1299_COMMAND_DEACTIVATE_LOFF_NCHAN = 'Z', /**< deactivates the lead-off detection for a n-channel */
	ADS1299_COMMAND_DEACTIVATE_LOFF_PCHAN = 'Q', /**< deactivates the lead-off detection for p n-channel */
} ads1299_command_cpt;

#define ADS1299_REG_CH_0 0x04

typedef enum {
	ADS1299_REG_CONFIG1 = 0x01,
	ADS1299_REG_CONFIG2 = 0x02,
	ADS1299_REG_CONFIG3 = 0x03,
	ADS1299_REG_LOFF = 0x04,
	ADS1299_REG_BIAS_SENSP = 0x0D,
	ADS1299_REG_BIAS_SENSN = 0x0E,
	ADS1299_REG_LOFF_SENSP = 0x0F,
	ADS1299_REG_LOFF_SENSN = 0x10,
	ADS1299_REG_LOFF_FLIP = 0x11,
	ADS1299_REG_BIAS_MISC1 = 0x15
} ads1299_registers;

typedef struct {
	datastream_source_t super;

	const data_port_t* data_out;
	control_port_t control_in;

	spi_master_regs_t* spi_master;

	int running;
	int use_srb2;
	int auto_bias;
} device_ads1299_t;

void device_ads1299_init(device_ads1299_t* const ads1299,
		spi_master_regs_t* const spi_master, const int drdy_id, ads1299_srb2 srb2,
		ads1299_data_rate sample_rate);

void device_ads1299_set_data_out(device_ads1299_t* const ads1299,
		const data_port_t* const data_in);

control_port_t* device_ads1299_get_control_in(device_ads1299_t* const ads1299);

void device_ads1299_configure_channel(device_ads1299_t* const ads1299,
		int channel, ads1299_ach_active active, ads1299_pga pga, ads1299_mux mux,
		ads1299_srb2 srb2);

void device_ads1299_configure_all_channels(device_ads1299_t* const ads1299,
		ads1299_ach_active active, ads1299_pga pga, ads1299_mux mux,
		ads1299_srb2 srb2);

void device_ads1299_set_auto_bias_generation(device_ads1299_t* const ads1299,
		ads1299_auto_bias state);

void device_ads1299_config_test_signal(device_ads1299_t* const ads1299,
		ads1299_test_sig_amp amp, ads1299_test_sig_freq freq);

void device_ads1299_config_loff(device_ads1299_t* const ads1299,
		ads1299_loff_freq frequency, ads1299_loff_mag magnitude,
		ads1299_loff_thres threshold);

void device_ads1299_change_channel_loff(device_ads1299_t* const ads1299,
		int channel, ads1299_loff_config setting);

#endif
