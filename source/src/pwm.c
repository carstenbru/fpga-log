/**
 * @file pwm.c
 * @brief pwm peripheral functions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <peripherals/pwm.h>
#include <stdint.h>

#include "peripheral_funcs/pwm.h"
#include "long_int.h"

void pwm_config_channels(pwm_regs_t* const pwm, unsigned int channels,
		unsigned int frequency, unsigned char duty, int phase) {
	unsigned long int period = PERI_CLOCK;
	period /= cast_to_ulong(frequency);

	unsigned long int calc = period - 1;  //correct period since pwm stages make one tick more

	pwm->config = PWM_PERIOD;
	pwm->data_low = (unsigned int) calc;
	pwm->data_high = (unsigned int) (calc >> 18);
	pwm->pwm_select = channels;

	calc = mul34_17(period, duty);
	calc /= 100;

	pwm->config = PWM_ON_TIME;
	pwm->data_low = (unsigned int) calc;
	pwm->data_high = (unsigned int) (calc >> 18);
	pwm->pwm_select = channels;

	calc = mul34_17(period, phase);
	calc /= 360;

	pwm->config = PWM_PHASE;
	pwm->data_low = (unsigned int) calc;
	pwm->data_high = (unsigned int) (calc >> 18);
	pwm->pwm_select = channels;
}
