/**
 * @file print_long.c
 * @brief long integer print function
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <stdio.h>
#include <fpga-log/long_int.h>

void print_long(unsigned long int val, unsigned int minwidth,
		unsigned int maxwidth) {
	unsigned long int old;
	char buf[13];
	buf[12] = 0;
	char* p = &buf[12];
	while (val || minwidth) {
		old = val;
		val /= 10;
		*(--p) = ((unsigned char) (old - val * 10)) + '0';
		if (minwidth)
			minwidth--;
		maxwidth--;
		if (!maxwidth)
			break;
	}
	putsn(p);
}
