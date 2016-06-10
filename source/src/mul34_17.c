/**
 * @file mul34_17.c
 * @brief 34-bit * 17-bit multiplication (truncated to 34-bit)
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/long_int.h>

unsigned long int mul34_17(unsigned long int a_34, unsigned int b_17) {
	unsigned long int result = 0;

	unsigned int low, high;

	low = a_34;
	high = a_34 >> 17;

	low &= ~(1 << 17);  //make sure all operands are positive
	b_17 &= ~(1 << 17);
	high &= ~(1 << 17);

	__asm__("MUL	%1,	%2\n\t"
			"MUL	%0,	%2\n\t"
			"MOVS2I	%2,	SFR_MUL\n\t"
			"SLLI %2, $1\n\t"
			"ADDU	%1,	%2"
			:"+r"(low), "+r"(high): "r"(b_17));

	result = cast_to_ulong(high);
	result <<= 17;
	result |= cast_to_ulong(low);

	return result;
}
