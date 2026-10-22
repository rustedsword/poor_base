#include <poor_container_of.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct inner {
	short x;
	long y;
};

struct item {
	char c;
	int a;
	char name[8];
	struct inner in;
	int arr[4];
};

struct qualified_item {
	char c;
	const int id;
	volatile int flag;
	_Atomic int counter;
	int *restrict data;
};

struct small {
	int x;
	int a;
};

struct big {
	int x;
	int a;
	int b[4];
};

static struct item global;
static struct item *const global_ref = container_of(&global.in.y, struct item, in.y);

#define is_item_ref(ref) _Generic((ref), struct item *: true, default: false)

static int container_of_test(void) {
	struct item it = {0};
	assert(container_of(&it.c, struct item, c) == &it);
	assert(container_of(&it.a, struct item, a) == &it);
	assert(container_of(&it.name, struct item, name) == &it);
	assert(container_of(&it.name[3], struct item, name[3]) == &it);
	assert(container_of(&it.in.y, struct item, in.y) == &it);
	assert(container_of(&it.arr[2], struct item, arr[2]) == &it);
	assert(global_ref == &global);
	return 0;
}

static int container_of_qualifiers_test(void) {
	struct item it = {0};
	const struct item *c = &it;
	volatile struct item *v = &it;
	static_assert(is_item_ref(container_of(&c->a, struct item, a)));
	static_assert(is_item_ref(container_of(&v->name, struct item, name)));
	assert(container_of(&c->a, struct item, a) == &it);
	assert(container_of(&v->name, struct item, name) == &it);

	struct qualified_item q = {.id = 1};
	assert(container_of(&q.id, struct qualified_item, id) == &q);
	assert(container_of(&q.flag, struct qualified_item, flag) == &q);
	assert(container_of(&q.counter, struct qualified_item, counter) == &q);
	assert(container_of(&q.data, struct qualified_item, data) == &q);
	return 0;
}

static int container_of_single_eval_test(void) {
	struct item items[2] = {0};
	int i = 0;
	assert(container_of(&items[i++].a, struct item, a) == &items[0] && i == 1);
	return 0;
}

static int container_of_object_size_test(void) {
	struct item it = {0};
	assert(__builtin_object_size(container_of(&it.a, struct item, a), 0) == sizeof(it));

	struct item arr[4] = {0};
	struct item *ref = container_of(&arr[1].in.y, struct item, in.y);
	assert(__builtin_object_size(ref, 0) == sizeof(arr) - sizeof(arr[0]));
	assert(__builtin_object_size(ref->name, 1) == sizeof(ref->name));

	struct item *heap = malloc(3 * sizeof(*heap));
	assert(heap);
	ref = container_of(&heap[2].arr[1], struct item, arr[1]);
	assert(__builtin_dynamic_object_size(ref, 0) == sizeof(*heap));
	free(heap);
	return 0;
}

static int container_of_ubsan_test(void) {
	struct small *small = malloc(sizeof(*small));
	assert(small);
	volatile size_t index = 3;
	struct big *ref = container_of(&small->a, struct big, a);
	ref->b[index] = 1;
	free(small);
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(container_of_test),
	TEST_FN(container_of_qualifiers_test),
	TEST_FN(container_of_single_eval_test),
	TEST_FN(container_of_object_size_test),
	TEST_FN(container_of_ubsan_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
