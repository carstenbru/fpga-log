/**
 * @file uart_light_funcs.h
 * @brief additional functions for SpartanMC's uart_light peripheral
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
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

#endif
