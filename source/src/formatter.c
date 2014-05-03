 /**
 * @file formatter.c
 * @brief output log formatter
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <stdio.h>
#include "sink/formatter/formatter_simple.h"

void formatter_set_write_dest(
    formatter_t* formatter,
    void (*write_byte)(void *param, unsigned char byte),
    void* param) {
  formatter->write_byte = write_byte;
  formatter->param = param;
}
