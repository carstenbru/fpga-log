#include "stdio.h"

static const unsigned int hex2int_table[6] = {
    100000, 10000, 1000, 100, 10, 1
};

char *printf_format_uint(unsigned int value, int width, char fill) {
    unsigned int count = 0;
    char *digit = &printf_buffer[PRINTF_BUF_SIZE-7]; //-7 fuer 6 Stellen dezimal!
    unsigned int i;
    for (i=0; i<5; i++) {
        *digit = 48;
        while (value >= hex2int_table[i]) {
            value -= hex2int_table[i];
            (*digit)++;
            count++;
        }
        if (count) {
            digit++;
            width--;
        }
    }
    *digit = value + 48;
    digit++;
    *digit = 0;
    return printf_add_padding(&printf_buffer[PRINTF_BUF_SIZE-7], width - 1, fill);
}
