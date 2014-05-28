/**
 * @file device_pwm.h
 * @brief pwm device driver (only control in)
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef DEVICE_PWM_H_
#define DEVICE_PWM_H_

#include "peripherals/pwm.h"
#include "control_port.h"

#define DEVICE_PWM_START_FREQUENCY 100

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

#endif
