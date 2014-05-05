#include "stdio.h"

int putsn(const char *s) {
    while (*s) {
        putchar(*s);
        s++;
    }
    return 0;
}
