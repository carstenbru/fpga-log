#pragma GCC push_options
#pragma GCC optimize ("Os")

char *printf_add_padding(char *digit, int width, char fill) {
    while (width > 0) {
        digit--;
        width--;
        *digit = fill;
    }
    return digit;
}

#pragma GCC pop_options

