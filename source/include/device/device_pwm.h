/**
 * @file device_pwm.h
 * @brief pwm device driver (only control in)
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DEVICE_PWM_H_
#define DEVICE_PWM_H_

#include <peripherals/pwm.h>
#include <fpga-log/control_port.h>

#define DEVICE_PWM_START_FREQUENCY 100

/**
 * @brief enumeration of the different control parameters of the PWM device
 */
typedef enum {
	PWM_PARAMETER_CHANNEL = 'c', /**< channel */
	PWM_PARAMETER_FREQUENCY = 'f', /**< frequency */
	PWM_PARAMETER_DUTY = 'd', /**< duty-cycle */
	PWM_PARAMETER_PHASE = 'p', /**< phase to reference signal */
	PWM_PARAMETER_BANK = 'b' /**< bank (only needed for configuring multiple channels at once) */
} pwm_parameter_cpt;

/**
 * @brief struct describing a pwm device
 */
typedef struct {
	control_port_t control_in; /**< control port, this can be set at a control output to direct the control stream to this device */

	unsigned int last_freq;

	pwm_regs_t* pwm; /**< pointer to PWM hardware registers */
} device_pwm_t;

/**
 * @brief pwm device init function
 *
 * Initializes the pwm device, should be called before using the device.
 *
 * @param _pwm_device	pointer to the pwm device
 * @param pwm_regs		pointer to a pwm peripheral
 */
void device_pwm_init(device_pwm_t* const _pwm_device, pwm_regs_t* const pwm_regs);

/**
 * @brief returns the PWM device control input
 *
 * @param pwm		 pointer to the PWM device
 * @return the control input port
 */
control_port_t* device_pwm_get_control_in(device_pwm_t* const pwm);

#endif
