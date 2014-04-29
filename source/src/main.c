#include <system.h>
#include <system/peripherals.h>
#include <uart.h>

#include "data_port.h"

void main() {
  char c = 'h';
  while (1) {
    uart_light_receive_nb(UART_LIGHT_PC, &c);
    uart_light_send(UART_LIGHT_PC, c);
  }
}
