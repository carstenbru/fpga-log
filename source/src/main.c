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

#include "device/device_uart_raw.h"

/**
 * @brief main function
 */
void main() {
  device_uart_raw_t uart_raw = new_device_uart_raw(UART_LIGHT_PC, 1);
  
  
  while (1) {
     device_uart_raw_update(&uart_raw);
  }
}

/*
void main() {
  char c = 'h';
  while (1) {
    uart_light_receive_nb(UART_LIGHT_PC, &c);
    uart_light_send(UART_LIGHT_PC, c);
  }
}
*/