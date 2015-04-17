/**
 * @file umodsi3.c
 * @brief unsigned long int modulo function needed by gcc
 */

#include "fpga-log/long_int.h"

unsigned long __umodsi3(unsigned long a, unsigned long b) {
	return a - __udivsi3(a, b) * b;
}
