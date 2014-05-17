/**
 * @file pc_compatibility.c
 * @brief code needed to run and simulate the code natively on a PC
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <peripherals/compare.h>

#include "pc_native/pc_compatibility.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

file_pipe_t pipes[PIPE_COUNT];

io_descr_t stdio_descr = { };

//TODO PWM simulation?
pwm_regs_t pwm;
pwm_regs_t *const PWM_0 = &pwm;

/**
 * @brief initializes the pipe peripheral simulation
 */
void pipes_init(void) {
	char buffer[1024];

	int i;
	for (i = 0; i < PIPE_COUNT; i++) {
		sprintf(buffer, "fifo_in_%d", i);  //SpartanMC stdio.h overwrites normal c-header, but sprintf is there
		mkfifo(buffer, S_IRWXU | S_IRWXG);
		pipes[i].in = open(buffer, O_RDWR);
		int flags = fcntl(pipes[i].in, F_GETFL, 0);
		fcntl(pipes[i].in, F_SETFL, flags | O_NONBLOCK);

		sprintf(buffer, "fifo_out_%d", i);
		mkfifo(buffer, S_IRWXU | S_IRWXG);
		pipes[i].out = open(buffer, O_RDWR);
	}
}

void sys_init(void) {
	pipes_init();
}
