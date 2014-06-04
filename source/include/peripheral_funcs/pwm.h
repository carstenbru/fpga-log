/**
 * @file pwm.h
 * @brief pwm peripheral functions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef PWM_H_
#define PWM_H_

#include <peripherals/pwm.h>

#define DUTY_MAX 100
#define PHASE_MAX 360

/**
 * @brief configures (multiple) pwm channels
 *
 * @param pwm 			pointer to the pwm peripheral
 * @param bank			bank of the channels to configure (channel 0-17 are bank 1, 18-35 bank 2,...)
 * @param channels 	mask with channels to configure (every bit represents one channel)
 * @param frequency	frequency of the pwm signal in Hz
 * @param	duty			duty cycle of the pwm signal in %
 * @param	phase			phase of the pwm signal to the (internal) reference signal in degree (normally 0-360 degree, also negative values possible)
 */
void pwm_config_channels(pwm_regs_t* const pwm, unsigned int bank,
		unsigned int channels, unsigned int frequency, unsigned char duty,
		int phase);

/**
 * @brief configures a single pwm channel
 *
 * @param pwm				pointer to the pwm peripheral
 * @param channel		number of the channel to configure
 * @param frequency frequency of the pwm signal in Hz
 * @param duty			duty cycle of the pwm signal in %
 * @param phase			phase of the pwm signal to the (internal) reference signal in degree (normally 0-360 degree, also negative values possible)
 */
void pwm_config_single_channel(pwm_regs_t* const pwm, unsigned int channel,
		unsigned int frequency, unsigned char duty, int phase);

#endif
