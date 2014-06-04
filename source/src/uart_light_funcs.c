/**
 * @file src/uart_light_funcs.c
 * @brief additional functions for SpartanMC's uart_light peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "peripheral_funcs/uart_light_funcs.h"

void uart_light_enable_rxint(uart_light_regs_t* uart) {
	uart->status |= UART_LIGHT_RXIE;
}

