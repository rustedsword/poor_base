#include <poor_asan.h>

void poison(unsigned long addr);

void poison(unsigned long addr) {
	poor_asan_poison_region(addr, 8);
}
