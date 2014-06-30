/**
 * @file src/pwm.c
 * @brief pwm peripheral functions
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <peripherals/pwm.h>
#include <stdint.h>

#include <fpga-log/peripheral_funcs/pwm.h>
#include <fpga-log/long_int.h>

void pwm_config_channels(pwm_regs_t* const pwm, unsigned long int peri_clock,
		unsigned int bank, unsigned int channels, unsigned int frequency,
		unsigned char duty, int phase) {
	bank <<= 2;

	peri_clock /= cast_to_ulong(frequency);

	unsigned long int calc = peri_clock - 1;  //correct period since pwm stages make one tick more

	pwm->config = PWM_PERIOD | bank;
	pwm->data_low = (unsigned int) calc;
	pwm->data_high = (unsigned int) (calc >> 18);
	pwm->pwm_select = channels;

	calc = mul34_17(peri_clock, duty);
	calc /= DUTY_MAX;

	pwm->config = PWM_ON_TIME | bank;
	pwm->data_low = (unsigned int) calc;
	pwm->data_high = (unsigned int) (calc >> 18);
	pwm->pwm_select = channels;

	phase = PHASE_MAX - (phase % PHASE_MAX);
	calc = mul34_17(peri_clock, phase);
	calc /= PHASE_MAX;

	pwm->config = PWM_PHASE | bank;
	pwm->data_low = (unsigned int) calc;
	pwm->data_high = (unsigned int) (calc >> 18);
	pwm->pwm_select = channels;
}

void pwm_config_single_channel(pwm_regs_t* const pwm,
		unsigned long int peri_clock, unsigned int channel, unsigned int frequency,
		unsigned char duty, int phase) {
	pwm_config_channels(pwm, peri_clock, channel / 18, (1 << (channel % 18)),
			frequency, duty, phase);
}
