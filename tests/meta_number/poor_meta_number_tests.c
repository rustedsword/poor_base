#include <poor_meta_number.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

typedef union { int raw; poor_meta_number_8(8); } q8;
typedef union { int raw; poor_meta_number_8(16); } q16;

#define fixed_to_double(x) ((double)(x).raw / (1ull << poor_meta_number_8_get(x)))

struct job { char name[300]; int id; };
typedef union { char c; poor_meta_number(offsetof(struct job, id)) id_offset; } small;
typedef poor_meta_number(42) answer;
typedef union {
	char c;
	poor_meta_number(1); poor_meta_number_32(2); poor_meta_number_16(3); poor_meta_number_8(4);
} variants;

#define round_trip(value)												\
	static_assert(poor_meta_number_get(poor_meta_number(value)) == (unsigned long long)(value), #value);		\
	static_assert(poor_meta_number_32_get(poor_meta_number_32((unsigned long long)(value) & 0xffffffff))	\
		      == ((unsigned long long)(value) & 0xffffffff), #value);						\
	static_assert(poor_meta_number_16_get(poor_meta_number_16((unsigned long long)(value) & 0xffff))	\
		      == ((unsigned long long)(value) & 0xffff), #value);						\
	static_assert(poor_meta_number_8_get(poor_meta_number_8((unsigned long long)(value) & 0xff))		\
		      == ((unsigned long long)(value) & 0xff), #value);

static int meta_number_example_test(void) {
	q8 a = {.raw = 3 * 256 + 128};
	q16 b = {.raw = 1 << 15};
	q8 c = a;

	assert(fixed_to_double(a) == 3.5 && fixed_to_double(b) == 0.5 && fixed_to_double(c) == 3.5);
	return 0;
}

static int meta_number_values_test(void) {
	round_trip(0) round_trip(1) round_trip(7) round_trip(8) round_trip(63) round_trip(64)
	round_trip(ULLONG_MAX) round_trip(1ull << 63) round_trip(UINT32_MAX) round_trip(UINT32_MAX + 1ull)
	round_trip(00432107654321076543210) round_trip(01543210765432107654321) round_trip(00654321076543210765432)
	round_trip(01765432107654321076543) round_trip(00076543210765432107654) round_trip(01107654321076543210765)
	round_trip(00210765432107654321076) round_trip(01321076543210765432107)
	round_trip(-1) round_trip(LLONG_MIN) round_trip(true) round_trip('a') round_trip((unsigned char)200)
	round_trip(-1wb) round_trip((unsigned _BitInt(64))-1) round_trip(SIZE_MAX)

	static_assert(poor_meta_number_get(poor_meta_number(-1)) == ULLONG_MAX);
	return 0;
}

static int meta_number_key_test(void) {
	struct job job = {.id = 7};
	small s = {.c = 'x'};
	const volatile small *p = &s;
	_Atomic q8 *atomic = nullptr;
	int evaluated = 0, id;

	static_assert(poor_meta_number_get(answer) == 42 && poor_meta_number_get(const answer) == 42);
	static_assert(poor_meta_number_get(s.id_offset) == offsetof(struct job, id));
	static_assert(poor_meta_number_get(p->id_offset) == offsetof(struct job, id));
	static_assert(poor_meta_number_8_get(*atomic) == 8 && poor_meta_number_8_get(q16) == 16);
	static_assert(poor_meta_number_get(variants) == 1 && poor_meta_number_32_get(variants) == 2
		      && poor_meta_number_16_get(variants) == 3 && poor_meta_number_8_get(variants) == 4);
	static_assert(poor_meta_number_get((evaluated++, s).id_offset) == offsetof(struct job, id));

	memcpy(&id, (char *)&job + poor_meta_number_get(s.id_offset), sizeof(id));
	assert(id == 7 && !evaluated && s.c == 'x');
	return 0;
}

static int meta_number_constant_test(void) {
	int numbers[poor_meta_number_get(answer)];
	enum { e = poor_meta_number_get(answer) };
	constexpr unsigned long long c = poor_meta_number_get(answer);
	struct { unsigned bits : poor_meta_number_8_get(q16); } field = {.bits = 65535};
	int value = 0;

	static_assert(_Generic(poor_meta_number_get(answer), unsigned long long: 1, default: 0));
	switch(value) {
	case poor_meta_number_get(answer):
		return 1;
	default:
		assert(sizeof(numbers) == 42 * sizeof(int) && e == 42 && c == 42 && field.bits == 65535);
	}
	return 0;
}

static int meta_number_layout_test(void) {
	struct holder { char a; answer n; char b; };

	static_assert(sizeof(answer) == 1 && alignof(answer) == 1);
	static_assert(sizeof(small) == 1 && alignof(small) == 1 && sizeof(small[4]) == 4);
	static_assert(sizeof(q8) == sizeof(int) && alignof(q8) == alignof(int));
	static_assert(sizeof(variants) == 1 && alignof(variants) == 1);
	static_assert(sizeof(struct holder) == 3 && alignof(struct holder) == 1);
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(meta_number_example_test),
	TEST_FN(meta_number_values_test),
	TEST_FN(meta_number_key_test),
	TEST_FN(meta_number_constant_test),
	TEST_FN(meta_number_layout_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
