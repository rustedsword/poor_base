#include <poor_array.h>

/* Stringifying an expanded destination makes literals longer than the 4095 characters
 * -pedantic-errors allows. Nested views get there with GCC too; if the expansion shrinks
 * or the limit grows, this test no longer catches it. */
int main(void) {
	int dest[8] = {0};
	int src[3] = {1,2,3};
	copy_array(arrview(1, 3, arrview(1, 7, dest)), src);
	memset_array(arrview(1, 3, arrview(1, 7, dest)), 0);
	array_remove_view(arrview(0, 7, arrview(1, 7, dest)), arrview(2, 3, dest));
	array_remove_view_fill(arrview(0, 7, arrview(1, 7, dest)), arrview(2, 3, dest), 0);
	return 0;
}
