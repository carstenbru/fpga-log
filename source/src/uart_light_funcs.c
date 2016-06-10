/**
 * @file src/uart_light_funcs.c
 * @brief additional functions for SpartanMC's uart_light peripheral
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/peripheral_funcs/uart_light_funcs.h>

void uart_light_enable_rxint(uart_light_regs_t* uart) {
	uart->status |= UART_LIGHT_RXIE;
}

void uart_light_disable_rxint(uart_light_regs_t* uart) {
	uart->status &= ~UART_LIGHT_RXIE;
}

int uart_light_rx_data_available(uart_light_regs_t* uart) {
	return (uart->status & UART_LIGHT_RX_EMPTY) ? 0 : 1;
}
