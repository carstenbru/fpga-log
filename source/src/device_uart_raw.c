/**
 * @file device_uart_raw.c
 * @brief simple raw uart logging device driver
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "device/device_uart_raw.h"

void device_uart_raw_init(device_uart_raw_t* uart_raw, uart_light_regs_t* uart_light, int id) {
  uart_raw->control_in.parent = (void*)uart_raw;
  //TODO set function pointers of control_in here
  
  uart_raw->uart_light = uart_light;
}

void device_uart_raw_set_data_out(device_uart_raw_t* uart_raw, data_port_t* data_in) {
  uart_raw->data_out = data_in;
}

void device_uart_raw_update(device_uart_raw_t* uart_raw) {
  unsigned char byte;
  if (uart_light_receive_nb(uart_raw->uart_light, &byte) == UART_OK) {
    data_package_t package = { uart_raw->id, DATA_TYPE_BYTE, &byte };
    uart_raw->data_out->new_data(uart_raw->data_out->parent, &package);
  }
}