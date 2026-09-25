#include <poor_array.h>

int main(void) {
	int a[] = {0, 1, 2, 3, 4};
	array_insert(a, 2, 9);
	assert(a[0] == 0 && a[1] == 1 && a[2] == 9 && a[3] == 2 && a[4] == 3);
	return 0;
}
