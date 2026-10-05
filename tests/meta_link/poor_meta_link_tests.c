#include <poor_meta_link.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static double celsius_to_kelvin(signed char c) { return c + 273.15; }
static double fahrenheit_to_kelvin(signed char f) { return (f - 32) * 5 / 9.0 + 273.15; }

poor_meta_link_define(celsius_kelvin, celsius_to_kelvin);
poor_meta_link_define(fahrenheit_kelvin, fahrenheit_to_kelvin);

typedef union { signed char degrees; celsius_kelvin kelvin; } celsius;
typedef union { signed char degrees; fahrenheit_kelvin kelvin; } fahrenheit;
static_assert(sizeof(celsius) == 1 && alignof(celsius) == 1 && sizeof(fahrenheit) == 1);

#define kelvin(t) poor_meta_link_get((t).kelvin)((t).degrees)

static int twice(int x) { return 2 * x; }
poor_meta_link_define(twice_key, twice);

typedef union { int value; twice_key apply; } doubler;
static_assert(sizeof(doubler) == sizeof(int));

static int calls;
static void count_call(void) { calls++; }
poor_meta_link_define(count_key, count_call);

struct shape_ops { int (*area)(int); int (*perimeter)(int); };
static int square_area(int side) { return side * side; }
static int square_perimeter(int side) { return 4 * side; }
static const struct shape_ops square_ops = {square_area, square_perimeter};
poor_meta_link_define(square_ops_key, &square_ops);

typedef union { int side; square_ops_key ops; } square;
static_assert(sizeof(square) == sizeof(int));

static int table[] = {1, 2, 3};
poor_meta_link_define(table_key, table);
poor_meta_link_define(table_array_key, &table);
poor_meta_link_define(name_key, "poor");

struct point { int x, y; };
constexpr struct point origin = {1, 2};
constexpr int limit = 64;
poor_meta_link_define(answer_key, 42);
poor_meta_link_define(half_key, 0.5);
poor_meta_link_define(limit_key, limit);
poor_meta_link_define(origin_key, origin);

static int meta_link_example_test(void) {
	celsius c = {.degrees = 25};
	fahrenheit f = {.degrees = 77};

	printf("%.2f K, %.2f K\n", kelvin(c), kelvin(f));
	assert(kelvin(c) == kelvin(f));
	return 0;
}

static int meta_link_key_test(void) {
	const doubler d = {.value = 21};
	const doubler *p = &d;
	int (*fn)(int) = poor_meta_link_get(p->apply);
	typeof(poor_meta_link_get(twice_key)) cb = nullptr;
	int evaluated = 0;

	cb = poor_meta_link_get(twice_key);
	assert(fn == twice && cb == twice);
	assert(poor_meta_link_get(p->apply)(p->value) == 42);
	assert(poor_meta_link_get(twice_key)(5) == 10);
	assert(poor_meta_link_get((evaluated++, d).apply)(1) == 2 && !evaluated);
	poor_meta_link_get(count_key)();
	assert(calls == 1);
	return 0;
}

static int meta_link_block_scope_test(void) {
	poor_meta_link_define(local_key, twice);
	poor_meta_link_define(unused_key, twice);

	static_assert(sizeof(unused_key) == 1);
	assert(poor_meta_link_get(local_key)(4) == 8);
	return 0;
}

static int meta_link_object_test(void) {
	static int hits;
	poor_meta_link_define(hits_key, &hits);
	square s = {.side = 3};

	(*poor_meta_link_get(hits_key))++;
	assert(hits == 1);
	assert(poor_meta_link_get(s.ops)->area(s.side) == 9 && poor_meta_link_get(s.ops)->perimeter(s.side) == 12);
	assert(poor_meta_link_get(table_key)[2] == 3 && !strcmp(poor_meta_link_get(name_key), "poor"));
	return 0;
}

static int meta_link_array_test(void) {
	static_assert(_Generic(poor_meta_link_get(table_key), int *: 1, default: 0));
	static_assert(_Generic(poor_meta_link_get(table_array_key), int (*)[3]: 1, default: 0));
	static_assert(sizeof(*poor_meta_link_get(table_array_key)) == sizeof(table));

	assert(poor_meta_link_get(table_array_key) == &table && (*poor_meta_link_get(table_array_key))[2] == 3);
	return 0;
}

static int meta_link_value_test(void) {
	static_assert(_Generic(poor_meta_link_get(answer_key), int: 1, default: 0));
	assert(poor_meta_link_get(answer_key) == 42 && poor_meta_link_get(half_key) == 0.5);
	assert(poor_meta_link_get(limit_key) == 64 && poor_meta_link_get(origin_key).y == 2);
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(meta_link_example_test),
	TEST_FN(meta_link_key_test),
	TEST_FN(meta_link_block_scope_test),
	TEST_FN(meta_link_object_test),
	TEST_FN(meta_link_array_test),
	TEST_FN(meta_link_value_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
