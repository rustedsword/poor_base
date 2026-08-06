#include <poor_array.h>

int main(void) {
	const int dest[4] = {9,9,9,9};
	int a[2] = {1,2};
	int b[2] = {3,4};
	copy_arrays(dest, a, b);
	return 0;
}
