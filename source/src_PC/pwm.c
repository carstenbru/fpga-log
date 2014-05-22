/**
 * @file src_PC/pwm.c
 * @brief native PC functions to simulate pwm peripheral
 *
 * The simulation prints the configuration calls to a pipe.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include <unistd.h>

#include "peripheral_funcs/pwm.h"
#include "pc_native/pc_compatibility.h"

void pwm_config_channels(pwm_regs_t* const pwm, unsigned int bank,
		unsigned int channels, unsigned int frequency, unsigned char duty,
		int phase) {
	char buffer[1024];

	//SpartanMC stdio.h overwrites normal c-header, but sprintf is there
	int i =
			sprintf(buffer,
					"pwm config: bank:%d\tchannels:%d\tfrequency:%dHz\tduty-cycle:%d%%\tphase:%d degree\n",
					bank, channels, frequency, duty, phase % 360);

	write((&pipes[(int_ptr) pwm])->out, &buffer, i);
}

void pwm_config_single_channel(pwm_regs_t* const pwm, unsigned int channel,
		unsigned int frequency, unsigned char duty, int phase) {
	pwm_config_channels(pwm, channel / 18, (1 << (channel % 18)), frequency, duty,
			phase);
}
