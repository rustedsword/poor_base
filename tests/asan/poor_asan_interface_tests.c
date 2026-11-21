#if __has_include(<sanitizer/asan_interface.h>)
#include <sanitizer/asan_interface.h>
#endif
#include <poor_asan.h>
#include "poor_asan_tests.h"
#include <assert.h>

int asan_interface_test(void);

int asan_interface_test(void) {
	alignas(8) static unsigned char buf[16];
#ifdef ASAN_POISON_MEMORY_REGION
	ASAN_POISON_MEMORY_REGION(buf, sizeof(buf));
	assert(poor_asan_is_poisoned(buf) == asan_enabled);
	poor_asan_poison_region(buf, sizeof(buf));
	ASAN_UNPOISON_MEMORY_REGION(buf, sizeof(buf));
#endif
	assert(!poor_asan_is_poisoned(buf));
	return 0;
}
