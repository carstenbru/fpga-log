/**
 * @file addu36.c
 * @brief 36-bit + 36-bit addition
 *
 * @author Stefan Klir (s.klir@gmx.de)
 */

#include <fpga-log/long_int.h>

//SpartanMC linker bug, if the calculation is incorrect the linker may has some problems
unsigned long int addu36(unsigned long int a_36, unsigned long int b_36) {
	unsigned long int result = 0L;

	unsigned int a_low, a_high;
	unsigned int b_low, b_high;


	a_low = a_36;
	a_high = a_36 >> 18;
	b_low = b_36;
	b_high = b_36 >> 18;

	__asm__("ADDU	%0,	%2\n\t"
			"ADDU	%1,	%3\n\t"
			"SLTU	%1, %3\n\t"
			"MOVS2I	%3,	SFR_CC\n\t"
			"SLTU	%0, %2\n\t"
			"MOVS2I	%2,	SFR_CC\n\t"
			"ADDU   %1, %2\n\t"
			"SLTU   %1, %2\n\t"
			"MOVS2I %2, SFR_CC\n\t"
			"ADDU   %2, %3"
			:"+r"(a_low), "+r"(a_high): "r"(b_low), "r"(b_high) );


	result = cast_to_ulong(a_high);
	result <<= 18;
	result |= cast_to_ulong(a_low);

	return result;
}
