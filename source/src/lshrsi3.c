/**
 * @file lshrsi3.c
 * @brief unsigned long int logical shift right function needed by gcc
 */

//TODO faster implementation
unsigned long int __lshrsi3(unsigned long int a, short int b) {
	while (b--)
		a >>= 1;
	return a;
}
