/**
 * @file divsi3.c
 * @brief long int division function needed by gcc
 *
 * from: http://opensource.apple.com/source/clang/clang-137/src/projects/compiler-rt/lib/divsi3.c
 */

#define DIV_SIZE sizeof(long int)*9-1

long int __attribute__((optimize("Os"))) __divsi3(long int a, long int b) {
	long s_a = a >> DIV_SIZE; /* s_a = a < 0 ? -1 : 0 */
	long s_b = b >> DIV_SIZE; /* s_b = b < 0 ? -1 : 0 */
	a = (a ^ s_a) - s_a; /* negate if s_a == -1 */
	b = (b ^ s_b) - s_b; /* negate if s_b == -1 */
	s_a ^= s_b; /* sign of quotient */
	return (__udivsi3(a, b) ^ s_a) - s_a; /* negate if s_a == -1 */
}
