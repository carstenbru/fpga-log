/**
 * @file device_pwm.c
 * @brief pwm device driver (only control in)
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_pwm.h>
#include <fpga-log/peripheral_funcs/pwm.h>
#include <fpga-log/sys_init.h>

/**
 * @brief control message function of pwm device
 *
 * @param	parent			the pwm device
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void device_pwm_control_message(void* const pwm_device,
		unsigned int count, const control_parameter_t* parameters);

void device_pwm_init(device_pwm_t* const pwm_device, pwm_regs_t* const pwm_regs) {
	pwm_device->pwm = pwm_regs;

	pwm_device->control_in = control_port_dummy;
	pwm_device->control_in.parent = (void*) pwm_device;
	pwm_device->control_in.new_control_message = device_pwm_control_message;

	pwm_device->last_freq = DEVICE_PWM_START_FREQUENCY;
}

control_port_t* device_pwm_get_control_in(device_pwm_t* const pwm) {
	return &pwm->control_in;
}

static void device_pwm_control_message(void* const _pwm_device,
		unsigned int count, const control_parameter_t* parameters) {
	device_pwm_t* pwm_device = (device_pwm_t*) _pwm_device;

	unsigned int channel = 0;
	int bank = -1;
	unsigned int frequency = pwm_device->last_freq;
	unsigned char duty = 50;
	int phase = 0;

	while (count--) {
		switch (parameters->type) {
		case PWM_PARAMETER_CHANNEL:
			channel = parameters->value;
			break;
		case PWM_PARAMETER_BANK:
			bank = parameters->value;
			break;
		case PWM_PARAMETER_FREQUENCY:
			frequency = parameters->value;
			pwm_device->last_freq = frequency;
			break;
		case PWM_PARAMETER_DUTY:
			duty = parameters->value;
			break;
		case PWM_PARAMETER_PHASE:
			phase = parameters->value;
			break;
		}
		parameters++;
	}

	if (bank == -1) {
		pwm_config_single_channel(pwm_device->pwm, get_peri_clock(), channel, frequency, duty, phase);
	} else {
		pwm_config_channels(pwm_device->pwm, get_peri_clock(), bank, channel, frequency, duty, phase);
	}
}
