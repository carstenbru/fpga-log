#include "stdio.h"
#include "linkvars.h"

int putchar(const char c) {
    stdio_descr.send_byte(stdio_descr.base_adr, c);
    return (int)c;
}
