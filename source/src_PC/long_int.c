/**
 * @file long_int.c
 * @brief long int support functions
 *
 * These functions are needed to fix some problems in the SpartanMC long int support.
 * This is the PC implementation.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

unsigned long int cast_to_ulong(unsigned int i) {
	return i;
}

//TODO not really 34*17, change to 32*16?
unsigned long int mul34_17(unsigned long int a_34, unsigned int b_17) {
	return a_34 * b_17;
}
