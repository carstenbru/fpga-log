/**
 * @file main.c
 * @brief main source file containing main routine
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <system.h>
#include <system/peripherals.h>
#include <uart.h>

#include "data_port.h"

#include "device/uart_raw.h"

/**
 * @brief main function
 */
void main() {
  char c = 'h';
  while (1) {
    uart_light_receive_nb(UART_LIGHT_PC, &c);
    uart_light_send(UART_LIGHT_PC, c);
  }
}