#include <poor_array.h>

int main(void) {
	int **ptr = &(int*){&(int){0}};
	auto_arr(ptr)[0] = 2;
	return 0;
}
