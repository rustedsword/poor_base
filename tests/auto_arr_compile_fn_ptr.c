#include <poor_array.h>

int main(void) {
	void (*f)(void) = 0;
	auto_arr(f)[0] = 2;
	return 0;
}
