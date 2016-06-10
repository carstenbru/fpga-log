/**
 * @file uart_light_funcs.h
 * @brief additional functions for SpartanMC's uart_light peripheral
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef UART_LIGHT_FUNCS_H_
#define UART_LIGHT_FUNCS_H_

#include <peripherals/uart_light.h>

/**
 * @brief enables the rx interrupt of a uart_light
 */
void uart_light_enable_rxint(uart_light_regs_t* uart);
/**
 * @brief disables the rx interrupt of a uart_light
 */
void uart_light_disable_rxint(uart_light_regs_t* uart);

/**
 * @brief checks if the UART has received data in its FIFO
 *
 * @return true if data is available, otherwise false
 */
int uart_light_rx_data_available(uart_light_regs_t* uart);

#endif
