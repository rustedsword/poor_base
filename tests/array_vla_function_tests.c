#include "array_vla_function.h"
#include <assert.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	/* One, two, or three dimensions, then use_null, initial, and step. */
	assert(argc >= 5 && argc <= 7);
	size_t outer = strtoul(argv[1], NULL, 10);
	size_t second = argc >= 6 ? strtoul(argv[2], NULL, 10) : 1;
	size_t third = argc == 7 ? strtoul(argv[3], NULL, 10) : 1;
	assert(second > 0 && third > 0);
	size_t length = outer * second * third;
	bool use_null = atoi(argv[argc - 3]) != 0;
	int initial = atoi(argv[argc - 2]);
	int step = atoi(argv[argc - 1]);
	assert(!use_null || outer == 0);

	/* Nonempty arrays have an exact-sized allocation for ASan. A non-NULL
	 * empty array uses one sentinel element that must remain untouched. */
	size_t capacity = length ? length : 1;
	int *storage = use_null ? NULL : malloc(capacity * sizeof(*storage));
	if(!use_null) {
		assert(storage != NULL);
		for(size_t i = 0; i < capacity; i++)
			storage[i] = -999;
	}
	long sum;
	if(argc == 5) {
		int (*pointer)[outer] = (int (*)[outer])storage;
		sum = array_vla_function(outer, pointer, initial, step);
	} else if(argc == 6) {
		int (*pointer)[outer][second] = (int (*)[outer][second])storage;
		sum = array_vla_matrix_function(outer, second, pointer, initial, step);
	} else {
		int (*pointer)[outer][second][third] = (int (*)[outer][second][third])storage;
		sum = array_vla_cube_function(outer, second, third, pointer, initial, step);
	}

	long expected_sum = 0;
	for(size_t i = 0; i < length; i++) {
		int expected = initial + step * (int)(i + 1);
		assert(storage[i] == expected);
		expected_sum += expected;
	}
	assert(sum == expected_sum);
	if(!use_null && length == 0)
		assert(storage[0] == -999);
	free(storage);
	return 0;
}
