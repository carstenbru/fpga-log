/**
 * @file formatter.c
 * @brief output log formatter
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include "sink/formatter.h"

void formatter_test(void* formatter, data_package_t* package) {
  formatter_t* f = (formatter_t*)formatter;
  stdio_descr.base_adr = f->param;
  stdio_descr.send_byte = f->write_byte;
  
  printf("test: %d\n\r", *((unsigned char*)package->data));
}
