#include <poor_asan.h>

void poison(void *block);

void poison(void *block) {
	poor_asan_poison_object(block);
}
