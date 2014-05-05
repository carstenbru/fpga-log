#include <stdio.h>

char *printf_format_int(int value, int width, char fill) {
    char sign = 0;
    char *result;
    if (value < 0) {
        sign = '-';
        value = ~value;
        value++;
        if (width > 1) {
            width--;
        }
    }
    result = printf_format_uint(value, width, fill);
    if (sign) {
        result--;
        *result = sign;
    }
    return result;
}
