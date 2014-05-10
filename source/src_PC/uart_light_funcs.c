/**
 * @file uart_light_funcs.c
 * @brief native PC functions to simulate SpartanMC's uart light peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <uart.h>
#include <unistd.h>
#include "pc_native/pc_compatibility.h"

/**
 * @brief uart light recive function on PC (using pipes)
 *
 * @param uart		the number of the pipe
 * @param value		the received value
 * @return				UART_LIGHT_OK if new data was read, otherwise UART_LIGHT_NO_DATA
 */
int uart_light_receive_nb(uart_light_regs_t *uart, unsigned char *value) {
	return
			read((&pipes[(int_ptr) uart])->in, value, 1) > 0 ?
					UART_LIGHT_OK : UART_LIGHT_NO_DATA;
}

/**
 * @brief uart light send function on PC (using pipes)
 *
 * @param uart		the number of the pipe
 * @param value		the value to send
 */
void uart_light_send(uart_light_regs_t *uart, unsigned char value) {
	write((&pipes[(int_ptr) uart])->out, &value, 1);
}
