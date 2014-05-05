#include "stdio.h"

static const char digits[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

char *printf_format_bits(unsigned int value, const int shift, int width, char fill) {
    char *digit = &printf_buffer[PRINTF_BUF_SIZE-1];
    unsigned int mask = ((1 << shift) - 1);
    *digit = 0;
    while (value > 0) {
        digit--;
        width--;
        *digit = digits[value & mask];
        value >>= shift;
    }
    return printf_add_padding(digit, width, fill);
}
