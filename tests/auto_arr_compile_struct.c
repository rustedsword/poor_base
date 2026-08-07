#include <poor_array.h>

int main(void) {
	struct S { int a; } s = {0};
	auto_arr(s)[0] = 2;
	return 0;
}
