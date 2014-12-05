/**
 * @file ashrsi3.c
 * @brief long int arithmetic shift right function needed by gcc
 */

//TODO faster implementation
long int __ashrsi3(long int a, short int b) {
	while (b--)
		a >>= 1;
	return a;
}
