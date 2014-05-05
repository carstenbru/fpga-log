#include <stdio.h>

char printf_buffer[PRINTF_BUF_SIZE] = { 32, 32 };

int printf_(char *s, void *arg1, const void *arg2) {
    char *c;
    int arg_shift;
    while (*s) {
        if (*s == '%') {
            arg_shift = (s[1] != '%');
            c = printf_format_arg(&s, arg1);
            if (arg_shift) {
                arg1 = arg2;
            }
            putsn(c);
        } else {
            putchar(*s);
        }
        s++;
    }
    return 0;
}
