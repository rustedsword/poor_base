#include <poor_array.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Exercise GCC/Clang pointers to runtime-sized arrays, including a zero bound.
 * The backing storage has a fixed positive size: no zero-sized VLA is allocated.
 * Each case must visit exactly length elements; a zero bound must skip the body. */
#define CHECK_REF_ITERATOR(iterator, input, reverse) do { \
	size_t visited = 0; \
	iterator(input, ref) { \
		assert(visited < length); \
		size_t index = reverse ? length - 1 - visited : visited; \
		assert(ref == &storage[index]); \
		assert(*ref == storage[index]); \
		visited++; \
	} \
	assert(visited == length); \
} while (0)

#define DEFINE_REF_ITERATOR_TEST(iterator, reverse) \
static void iterator##_test(size_t length, bool use_null) { \
	int storage[] = {11, 13, 17}; \
	assert(length <= ARRAY_SIZE(storage)); \
	assert(!use_null || length == 0); \
	int (*pointer)[length] = use_null ? NULL : (void *)storage; \
	const int (*const_pointer)[length] = use_null ? NULL : (void *)storage; \
	assert(ARRAY_SIZE(pointer) == length); \
	CHECK_REF_ITERATOR(iterator, pointer, reverse); \
	CHECK_REF_ITERATOR(iterator, const_pointer, reverse); \
	if(use_null) { \
		CHECK_REF_ITERATOR(iterator, (int (*)[length])NULL, reverse); \
		CHECK_REF_ITERATOR(iterator, (const int (*)[length])NULL, reverse); \
	} else { \
		CHECK_REF_ITERATOR(iterator, (int (*)[length])storage, reverse); \
		CHECK_REF_ITERATOR(iterator, (const int (*)[length])storage, reverse); \
	} \
	assert(storage[0] == 11 && storage[1] == 13 && storage[2] == 17); \
}

DEFINE_REF_ITERATOR_TEST(foreach_array_ref, false)
DEFINE_REF_ITERATOR_TEST(foreach_array_const_ref, false)
DEFINE_REF_ITERATOR_TEST(foreach_array_ref_bw, true)
DEFINE_REF_ITERATOR_TEST(foreach_array_const_ref_bw, true)

#undef DEFINE_REF_ITERATOR_TEST
#undef CHECK_REF_ITERATOR

#define CHECK_INDEX_ITERATOR(input) do { \
	size_t visited = 0; \
	foreach_array_index(input, index) { \
		assert(visited < length); \
		assert(index == visited); \
		assert(auto_arr(input)[index] == storage[index]); \
		visited++; \
	} \
	assert(visited == length); \
} while (0)

static void foreach_array_index_test(size_t length, bool use_null) {
	int storage[] = {11, 13, 17};
	assert(length <= ARRAY_SIZE(storage));
	assert(!use_null || length == 0);
	int (*pointer)[length] = use_null ? NULL : (void *)storage;
	const int (*const_pointer)[length] = use_null ? NULL : (void *)storage;
	assert(ARRAY_SIZE(pointer) == length);
	CHECK_INDEX_ITERATOR(pointer);
	CHECK_INDEX_ITERATOR(const_pointer);
	if(use_null) {
		CHECK_INDEX_ITERATOR((int (*)[length])NULL);
		CHECK_INDEX_ITERATOR((const int (*)[length])NULL);
	} else {
		CHECK_INDEX_ITERATOR((int (*)[length])storage);
		CHECK_INDEX_ITERATOR((const int (*)[length])storage);
	}
}

#define CHECK_INDEX_ITERATOR_BW(input) do { \
	size_t visited = 0; \
	foreach_array_index_bw(input, index) { \
		assert(visited < length); \
		assert(index == length - 1 - visited); \
		assert(auto_arr(input)[index] == storage[index]); \
		visited++; \
	} \
	assert(visited == length); \
} while (0)

static void foreach_array_index_bw_test(size_t length, bool use_null) {
	int storage[] = {11, 13, 17};
	assert(length <= ARRAY_SIZE(storage));
	assert(!use_null || length == 0);
	int (*pointer)[length] = use_null ? NULL : (void *)storage;
	const int (*const_pointer)[length] = use_null ? NULL : (void *)storage;
	assert(ARRAY_SIZE(pointer) == length);
	CHECK_INDEX_ITERATOR_BW(pointer);
	CHECK_INDEX_ITERATOR_BW(const_pointer);
	if(use_null) {
		CHECK_INDEX_ITERATOR_BW((int (*)[length])NULL);
		CHECK_INDEX_ITERATOR_BW((const int (*)[length])NULL);
	} else {
		CHECK_INDEX_ITERATOR_BW((int (*)[length])storage);
		CHECK_INDEX_ITERATOR_BW((const int (*)[length])storage);
	}
}

#undef CHECK_INDEX_ITERATOR
#undef CHECK_INDEX_ITERATOR_BW

int main(int argc, char **argv) {
	assert(argc == 4);
	size_t length = strtoul(argv[2], NULL, 10);
	bool use_null = atoi(argv[3]) != 0;
	if(!strcmp(argv[1], "foreach_array_ref"))
		foreach_array_ref_test(length, use_null);
	else if(!strcmp(argv[1], "foreach_array_const_ref"))
		foreach_array_const_ref_test(length, use_null);
	else if(!strcmp(argv[1], "foreach_array_ref_bw"))
		foreach_array_ref_bw_test(length, use_null);
	else if(!strcmp(argv[1], "foreach_array_const_ref_bw"))
		foreach_array_const_ref_bw_test(length, use_null);
	else if(!strcmp(argv[1], "foreach_array_index"))
		foreach_array_index_test(length, use_null);
	else if(!strcmp(argv[1], "foreach_array_index_bw"))
		foreach_array_index_bw_test(length, use_null);
	else
		return 1;
	return 0;
}
