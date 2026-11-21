#include <poor_asan.h>
#include "poor_asan_tests.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct item {
	alignas(8) long long id;
	long long data[2];
};

static_assert(sizeof(struct item) == 24);

#define poisoned_if_asan(bytes) ((size_t)(bytes) * asan_enabled)

static size_t count_poisoned(const volatile void *addr, size_t bytes) {
	size_t count = 0;
	for(size_t i = 0; i < bytes; i++)
		count += poor_asan_is_poisoned((const volatile unsigned char *)addr + i);
	return count;
}

static int asan_region_test(void) {
	alignas(8) unsigned char buf[64] = {0};
	static_assert(_Generic(poor_asan_is_poisoned(buf), bool: 1, default: 0));
	assert(count_poisoned(buf, sizeof(buf)) == 0);

	poor_asan_poison_region(buf + 16, 32);
	assert(count_poisoned(buf, 16) == 0);
	assert(count_poisoned(buf + 16, 32) == poisoned_if_asan(32));
	assert(count_poisoned(buf + 48, 16) == 0);

	poor_asan_unpoison_region(buf + 16, 32);
	assert(count_poisoned(buf, sizeof(buf)) == 0);

	(poor_asan_poison_region(buf, 8), poor_asan_unpoison_region(buf, 8));
	assert(count_poisoned(buf, sizeof(buf)) == 0);
	return 0;
}

static int asan_object_test(void) {
	struct item items[4] = {0};

	poor_asan_poison_object(&items[1]);
	assert(count_poisoned(&items[0], sizeof(items[0])) == 0);
	assert(count_poisoned(&items[1], sizeof(items[1])) == poisoned_if_asan(sizeof(items[1])));
	assert(count_poisoned(&items[2], 2 * sizeof(items[2])) == 0);
	poor_asan_unpoison_object(&items[1]);
	assert(count_poisoned(items, sizeof(items)) == 0);

	poor_asan_poison_object(&items);
	assert(count_poisoned(items, sizeof(items)) == poisoned_if_asan(sizeof(items)));
	poor_asan_unpoison_object(&items);
	assert(count_poisoned(items, sizeof(items)) == 0);

	const volatile struct item *ref = &items[2];
	for(int poison = 1; poison >= 0; poison--) {
		if(poison)
			poor_asan_poison_object(ref);
		else
			poor_asan_unpoison_object(ref);
		assert(count_poisoned(items, sizeof(items)) == poisoned_if_asan(poison * (int)sizeof(*ref)));
	}
	return 0;
}

static int asan_vla_test(void) {
	size_t n = 2;
	alignas(8) long long store[6] = {0};
	long long (*grid)[n] = (void *)store;
	long long (*row)[n] = grid;

	poor_asan_poison_object(row++);
	assert(row == grid + 1);
	assert(count_poisoned(store, 2 * sizeof(store[0])) == poisoned_if_asan(2 * sizeof(store[0])));
	assert(count_poisoned(store + 2, 4 * sizeof(store[0])) == 0);

	poor_asan_unpoison_object(--row);
	assert(row == grid);
	assert(count_poisoned(store, sizeof(store)) == 0);
	return 0;
}

static int asan_single_eval_test(void) {
	struct item items[2] = {0};
	struct item *refs[] = {&items[1]};
	size_t sizes[] = {sizeof(items[1])};
	int i = 0, s = 0;

	poor_asan_poison_object(&items[i++]);
	assert(i == 1);
	poor_asan_unpoison_object(&items[--i]);
	assert(i == 0);

	poor_asan_poison_region(refs[i++], sizes[s++]);
	assert(i == 1 && s == 1);
	assert(poor_asan_is_poisoned(refs[--i]) == asan_enabled && i == 0);
	poor_asan_unpoison_region(refs[i++], sizes[--s]);
	assert(i == 1 && s == 0);
	assert(!poor_asan_is_poisoned(refs[--i]) && i == 0);
	return 0;
}

static int asan_use_after_poison_test(void) {
	if(!asan_enabled)
		return 77;

	static struct item it;
	struct item *volatile ref = &it;
	poor_asan_poison_object(ref);
	ref->id = 1;
	poor_asan_unpoison_object(ref);
	return 0;
}

typedef int test_fn (void);

extern test_fn asan_interface_test;

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(asan_region_test),
	TEST_FN(asan_object_test),
	TEST_FN(asan_vla_test),
	TEST_FN(asan_single_eval_test),
	TEST_FN(asan_interface_test),
	TEST_FN(asan_use_after_poison_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
