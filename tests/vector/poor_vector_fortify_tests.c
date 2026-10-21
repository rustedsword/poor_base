#include <poor_vector.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

poor_vector_declare(text_vector, uint8_t);
poor_vector_declare(note_vector, uint8_t);

struct message {
	int id;
	text_vector text;
	char buffer[20];
	int tail;
};

struct note {
	int id;
	char buffer[20];
	int tail;
	note_vector text;
};

poor_vector_attach_buffer(text_vector, struct message, text, buffer);
poor_vector_attach_buffer(note_vector, struct note, text, buffer);

#define TEXT "0123456789abcdefghi"
static_assert(sizeof(TEXT) == 20);

#define is_bounded(ptr, size, rest) ((size) <= __builtin_object_size(ptr, 1) && __builtin_object_size(ptr, 1) <= (rest) &&	\
	(size) <= __builtin_dynamic_object_size(ptr, 1) && __builtin_dynamic_object_size(ptr, 1) <= (rest))

#define check_strings(owner, third) do {										\
	strcpy(*poor_vector_buffer(&(owner)->text), TEXT);								\
	strcpy(poor_vector_at(&(owner)->text, third), &TEXT[3]);							\
	assert(!strcmp((owner)->buffer, TEXT));										\
	memset((owner)->buffer, 0, sizeof((owner)->buffer));								\
	snprintf(poor_vector_at(&(owner)->text, 0), sizeof(TEXT), "%s", TEXT);						\
	assert(!strcmp((owner)->buffer, TEXT));										\
} while(0)

/* The pointers go through variables, because object size builtins give up on arguments with function calls */
#define check_text(owner) do {												\
	size_t rest = sizeof(*(owner)) - offsetof(typeof(*(owner)), buffer);						\
	char *buffer = *poor_vector_buffer(&(owner)->text), *third = poor_vector_at(&(owner)->text, 3);		\
	assert(is_bounded(buffer, sizeof((owner)->buffer), rest) && is_bounded(third, sizeof((owner)->buffer) - 3, rest - 3));	\
	check_strings(owner, 3);											\
} while(0)

static int vector_fortify_local_test(void) {
	struct message m = {};
	struct note n = {};
	check_text(&m);
	check_text(&n);
	return 0;
}

static int vector_fortify_static_test(void) {
	static struct message m;
	static struct note n;
	check_text(&m);
	check_text(&n);
	return 0;
}

static int vector_fortify_heap_test(void) {
	struct message *m = calloc(1, sizeof(*m));
	struct note *n = calloc(1, sizeof(*n));
	assert(m && n);
	check_text(m);
	check_text(n);
	free(m);
	free(n);
	return 0;
}

static volatile size_t picked = 1;

/* GCC can't fold owners picked at runtime into their array, so it bounds them by what their address is computed from */
static int vector_fortify_picked_test(void) {
	struct message local[3] = {};
	static struct message global[3];
	struct message *heap = calloc(3, sizeof(*heap));
	assert(heap);
	size_t i = picked, third = picked + 2;
	check_strings(&local[i], 3);
	check_strings(&local[i], third);
	check_strings(&global[i], 3);
	check_strings(&global[i], third);
	check_strings(&heap[i], 3);
	check_strings(&heap[i], third);
	free(heap);
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(vector_fortify_local_test),
	TEST_FN(vector_fortify_static_test),
	TEST_FN(vector_fortify_heap_test),
	TEST_FN(vector_fortify_picked_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
