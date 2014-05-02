/**
 * @file sink_uart.c
 * @brief uart data sink, e.g. as interface to PC
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include "sink/sink_uart.h"

void sink_uart_init(sink_uart_t* sink_uart, uart_light_regs_t* uart_light) {
  sink_uart->data_in.parent = (void*)sink_uart;
  sink_uart->data_in.new_data = sink_uart_new_data;
  
  sink_uart->uart_light = uart_light;
  
  sink_uart->formatter.param = sink_uart->uart_light;
  sink_uart->formatter.write_byte = uart_light_send;
  
  sink_uart->formatter.format = formatter_test;
}

void sink_uart_set_control_out(sink_uart_t* sink_uart, control_port_t* control_in) {
  sink_uart->control_out = control_in;
}

void sink_uart_update(sink_uart_t* sink_uart) {
  //nothing to do here for now
  
  //TODO protocol for pc to invoke control functions of control_out
  //TODO multiple control_out's
}

void sink_uart_new_data(void* sink_uart, data_package_t* package) {
  sink_uart_t* sink = (sink_uart_t*)sink_uart;

  sink->formatter.format((void*)&(sink->formatter), package);
}