#include <poor_array.h>

int main(void) {
	const int dest[3] = {9,9,9};
	int src[3] = {1,2,3};
	copy_array(dest, src);
	return 0;
}
