/**
 * @file pwm.h
 * @brief pwm peripheral functions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef PWM_H_
#define PWM_H_

#include <peripherals/pwm.h>

/**
 * @brief configures (multiple) pwm channels
 *
 * @param pwm 			pointer to the pwm peripheral
 * @param channels 	mask with channels to configure (every bit represents one channel)
 * @param frequency	frequency of the pem signal in Hz
 * @param	duty			duty cycle of the pwm signal in %
 * @param	phase			phase of the pwm signal to the (internal) reference signal in degree (0-360)
 */
void pwm_config_channels(pwm_regs_t* const pwm, unsigned int channels,
		unsigned int frequency, unsigned char duty, int phase);

#endif
