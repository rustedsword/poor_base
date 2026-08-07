#include <poor_array.h>

int main(void) {
	void *ptr = &(int){1};
	auto_arr(ptr)[0] = 2;
	return 0;
}
