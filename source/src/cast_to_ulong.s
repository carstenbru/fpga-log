/**
 * casts a unsigned int to a unsigned long int
 *
 * This function is only needed because of a compiler bug.
 * Without explicitly using this function subreg optimization will fail.
 *
 * Problem occured on GNU C (GCC) version 4.4.5 (spartanmc).
 * Verify if still necessary on newer versions.
 */

.global cast_to_ulong
cast_to_ulong:
        ;low register (R4) is already correct since parameter is passed in R4
		jrs	r11 ;next instruction is executed due to delayed branch!
		xor r5,r5 ;set high register to zero
