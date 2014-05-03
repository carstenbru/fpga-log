/**
 * @file main.c
 * @brief main source file containing main routine
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <system.h>
#include <system/peripherals.h>
#include <uart.h>

#include "device/device_uart_raw.h"
#include "sink/sink_uart.h"
#include "sink/formatter/formatter_simple.h"

sink_uart_t sink_uart;
formatter_simple_t formatter_simple;

device_uart_raw_t uart_raw;

/**
 * @brief main function
 */
void main() {
  device_uart_raw_init(&uart_raw, UART_LIGHT_PC, 1);
  
  formatter_simple_init(&formatter_simple);
  sink_uart_init(&sink_uart, (formatter_t*)&formatter_simple, UART_LIGHT_PC);
  
  device_uart_raw_set_data_out(&uart_raw, &sink_uart.data_in); //connect the data_out of uart_raw device to the uart sink
  
  while (1) {
     device_uart_raw_update(&uart_raw);
     sink_uart_update(&sink_uart);
  }
}