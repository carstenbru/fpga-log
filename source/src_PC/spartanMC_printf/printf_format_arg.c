#include <stdio.h>

static char *percent = "%";
static char *invalid_format = "%?";

unsigned int printf_scan_int(const char *s) {
    unsigned int result;
    if ((*s >= '0') && (*s <= '9')) {
        result = (unsigned int)*s - 48;
        s++;
        if ((*s >= '0') && (*s <= '9')) {
            result = result * 10 + (unsigned int)*s - 48;
        }
        return result;
    }
    return 1;
}

char *printf_format_arg(char **s, int arg) {
    char fill;
    int width;
    (*s)++;
    if ((**s >= '0') && (**s <= '9')) {
        if (**s == '0') {
            fill = '0';
            (*s)++;
        } else {
            fill = ' ';
        }
        width = printf_scan_int(*s);
        while ((**s >= '0') && (**s <= '9')) {
            (*s)++;
        }
    } else {
        width = 1;
        fill = '0';
    }
    switch (**s) {
        case 'd':
            return printf_format_int(arg, width, fill);
        case 'u':
            return printf_format_uint((unsigned int)arg, width, fill);
        case 'b':
            return printf_format_bits((unsigned int)arg, 1, width, fill);
        case 'X':
        case 'x':
            return printf_format_bits((unsigned int)arg, 4, width, fill);
        case 'o':
            return printf_format_bits((unsigned int)arg, 3, width, fill);
        case 's':
            return (char*)arg;
        case '%':
            return percent;
        case 0:
            (*s)--;
        default:
            return invalid_format;
    }
}
