/**
 * @file udivsi3.c
 * @brief unsigned long int division function needed by gcc
 *
 * from: http://knowledgefrontier.blogspot.com/2009/04/integer-division-algorithm-in-c.html
 */

#include "fpga-log/long_int.h"

#define DIV_SIZE sizeof(unsigned long int)*9-1

unsigned long int __attribute__((optimize("Os"))) __udivsi3(unsigned long int a,
		unsigned long int b) {
	short int index = DIV_SIZE;
	unsigned int c = 0;
	unsigned long int bit = 0;
	unsigned long int tmp = 0;
	unsigned long int result = 0;

	if (a == 0)
		return 0;

	while (!(a >> index))
		index--;

	bit = a >> index; /* &1 ? */
	tmp = bit;

	while (1) {
		if (tmp >= b)
			c = 1, tmp -= b;
		else
			c = 0;

		result = (result << 1) | c;

		index--;
		if (index == -1)
			break;
		bit = a >> index & 1;
		tmp = (tmp << 1) | bit; /*tmp is the residue */
	}

	return result;
}
