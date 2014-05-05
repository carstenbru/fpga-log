/**
 * @file pc_compatibility.c
 * @brief code needed to run and simulate the code natively on a PC
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "pc_native/pc_compatibility.h"

#include <uart.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/**
 * @brief array of the pipes simulating SpartanMC peripherals
 */
file_pipe pipes[PIPE_COUNT];

io_descr_t stdio_descr = { };

/**
 * @brief uart light recive function on PC (using pipes)
 *
 * @param uart		the number of the pipe
 * @param value		the received value
 * @return				UART_LIGHT_OK if new data was read, otherwise UART_LIGHT_NO_DATA
 */
int uart_light_receive_nb(uart_light_regs_t *uart, unsigned char *value) {
	return
			read((&pipes[(int) uart])->in, value, 1) > 0 ?
					UART_LIGHT_OK : UART_LIGHT_NO_DATA;
}

/**
 * @brief uart light send function on PC (using pipes)
 *
 * @param uart		the number of the pipe
 * @param value		the value to send
 */
void uart_light_send(uart_light_regs_t *uart, unsigned char value) {
	write((&pipes[(int) uart])->out, &value, 1);
}

void pc_native_init(void) {
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
