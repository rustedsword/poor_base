#include <poor_stdio.h>
#include <string.h>
#include <assert.h>

static int printf_fmt_char(void) {
	unsigned char uc = 1;
	signed char sc = 2;
	char c = 'e';

	const char *fmt = printf_specifier_string(0, uc, sc, c);
	assert(strcmp("%hhu%hhd%c", fmt) == 0);

	fmt = printf_specifier_string(1, uc, sc, c);
	assert(strcmp("%hhu%hhd%c\n", fmt) == 0);

	return 0;
}

static int printf_fmt_short(void) {
	unsigned short us = 1;
	signed short ss = 2;

	const char *fmt = printf_specifier_string(0, us, ss);
	assert(strcmp("%hu%hd", fmt) == 0);

	fmt = printf_specifier_string(1, us, ss);
	assert(strcmp("%hu%hd\n", fmt) == 0);

	return 0;
}

static int printf_fmt_int(void) {
	unsigned int ui = 1;
	signed int si = 2;

	const char *fmt = printf_specifier_string(0, ui, si, 2, 3U);
	assert(strcmp("%u%d%d%u", fmt) == 0);

	fmt = printf_specifier_string(1, ui, si, 2, 3U);
	assert(strcmp("%u%d%d%u\n", fmt) == 0);

	return 0;
}

static int printf_fmt_long_int(void) {
	unsigned long ul = 1;
	signed long sl = 2;

	const char *fmt = printf_specifier_string(0, ul, sl, 2L, 2UL);
	assert(strcmp("%lu%ld%ld%lu", fmt) == 0);

	fmt = printf_specifier_string(1, ul, sl, 2L, 2UL);
	assert(strcmp("%lu%ld%ld%lu\n", fmt) == 0);

	return 0;
}

static int printf_fmt_llong_int(void) {
	unsigned long long ull = 1;
	signed long long sll = 2;

	const char *fmt = printf_specifier_string(0, ull, sll, 1LL, 1ULL);
	assert(strcmp("%llu%lld%lld%llu", fmt) == 0);

	fmt = printf_specifier_string(1, ull, sll, 1LL, 1ULL);
	assert(strcmp("%llu%lld%lld%llu\n", fmt) == 0);

	return 0;
}

static int printf_fmt_str(void) {
	char s1[] = "str1";
	const char s1c[] = "str1c";
	char *s2 = "str2";
	const char *s2c = "str2c";

	const char *fmt = printf_specifier_string(0, s1, s1c, s2, s2c, "literal");
	assert(strcmp("%s%s%s%s%s", fmt) == 0);

	fmt = printf_specifier_string(1, s1, s1c, s2, s2c, "literal");
	assert(strcmp("%s%s%s%s%s\n", fmt) == 0);

	return 0;
}

static int printf_fmt_real(void) {
	float f = 1;
	double d = 2;
	long double ld = 3;

	const char *fmt = printf_specifier_string(0, f, d, ld, 1.0f, 1.0, 1.0L);
	assert(strcmp("%f%lf%Lf%f%lf%Lf", fmt) == 0);

	fmt = printf_specifier_string(1, f, d, ld, 1.0f, 1.0, 1.0L);
	assert(strcmp("%f%lf%Lf%f%lf%Lf\n", fmt) == 0);

	return 0;
}

static int printf_fmt_hex_char(void) {
	unsigned char uc = 1;
	signed char sc = 2;

	const char *fmt = printf_specifier_string(0, fmt_hex(uc), fmt_hex(sc));
	assert(strcmp("%hhx%hhx", fmt) == 0);

	fmt = printf_specifier_string(1, fmt_hex(uc), fmt_hex(sc));
	assert(strcmp("%hhx%hhx\n", fmt) == 0);

	return 0;
}

static int printf_fmt_hex_short(void) {
	unsigned short us = 1;
	signed short ss = 2;

	const char *fmt = printf_specifier_string(0, fmt_hex(us), fmt_hex(ss));
	assert(strcmp("%hx%hx", fmt) == 0);

	fmt = printf_specifier_string(1, fmt_hex(us), fmt_hex(ss));
	assert(strcmp("%hx%hx\n", fmt) == 0);

	return 0;
}

static int printf_fmt_hex_int(void) {
	unsigned int ui = 1;
	signed int si = 2;

	const char *fmt = printf_specifier_string(0, fmt_hex(ui), fmt_hex(si), fmt_hex(1), fmt_hex(1U));
	assert(strcmp("%x%x%x%x", fmt) == 0);

	fmt = printf_specifier_string(1, fmt_hex(ui), fmt_hex(si), fmt_hex(1), fmt_hex(1U));
	assert(strcmp("%x%x%x%x\n", fmt) == 0);

	return 0;
}

static int printf_fmt_hex_long(void) {
	unsigned long ul = 1;
	signed long sl = 2;

	const char *fmt = printf_specifier_string(0, fmt_hex(ul), fmt_hex(sl), fmt_hex(1L), fmt_hex(1UL));
	assert(strcmp("%lx%lx%lx%lx", fmt) == 0);

	fmt = printf_specifier_string(1,  fmt_hex(ul), fmt_hex(sl), fmt_hex(1L), fmt_hex(1UL));
	assert(strcmp("%lx%lx%lx%lx\n", fmt) == 0);

	return 0;
}

static int printf_fmt_hex_llong(void) {
	unsigned long long ull = 1;
	signed long long sll = 2;

	const char *fmt = printf_specifier_string(0, fmt_hex(ull), fmt_hex(sll), fmt_hex(1LL), fmt_hex(1ULL));
	assert(strcmp("%llx%llx%llx%llx", fmt) == 0);

	fmt = printf_specifier_string(1,  fmt_hex(ull), fmt_hex(sll), fmt_hex(1LL), fmt_hex(1ULL));
	assert(strcmp("%llx%llx%llx%llx\n", fmt) == 0);

	return 0;
}

static int concat_vla_test(void) {

	concat_vla(string_vla,
		   (char)'e',
		   (unsigned char)1, (signed char)2,
		   (unsigned short)3, (signed short)4,
		   5, 6U,
		   7L, 8UL,
		   9LL, 10ULL,
		   "string",
		   true, false,
		   fmt_hex(20), fmt_hex(30U), fmt_hex(40U));

	assert(strcmp("e12345678910stringtruefalse141e28", string_vla) == 0);
	return 0;
}

static int concat_test(void) {
	char *string = 	concat((char)'e',
			       (unsigned char)1, (signed char)2,
			       (unsigned short)3, (signed short)4,
			       5, 6U,
			       7L, 8UL,
			       9LL, 10ULL,
			       "string",
			       true, false,
			       fmt_hex(20), fmt_hex(30U), fmt_hex(40U));

	assert(strcmp("e12345678910stringtruefalse141e28", string) == 0);
	free(string);
	return 0;
}

typedef int test_fn (void) ;

#define TEST_FN(fn) {STRINGIFY2(fn), fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(printf_fmt_char),
	TEST_FN(printf_fmt_short),
	TEST_FN(printf_fmt_int),
	TEST_FN(printf_fmt_long_int),
	TEST_FN(printf_fmt_llong_int),
	TEST_FN(printf_fmt_str),
	TEST_FN(printf_fmt_real),
	TEST_FN(printf_fmt_hex_char),
	TEST_FN(printf_fmt_hex_short),
	TEST_FN(printf_fmt_hex_int),
	TEST_FN(printf_fmt_hex_long),
	TEST_FN(printf_fmt_hex_llong),
	TEST_FN(concat_vla_test),
	TEST_FN(concat_test),
};

static void usage(void) {
	printerrln("usage: this_program [test_name]\n\n"
		   "available tests:");

	for(struct tests_struct *cur = &tests[0]; cur != &tests[sizeof(tests) / sizeof(tests[0])]; cur++ ){
		printerrln("\t", cur->test_name);
	}
}

int main(int argc, char **argv) {
	if(argc != 2)
		return usage(), EXIT_FAILURE;

	for(struct tests_struct *cur = &tests[0]; cur != &tests[sizeof(tests) / sizeof(tests[0])]; cur++ ){
		if(!strcmp(argv[1], cur->test_name)) {
			return cur->fn();
		}
	}

	return printerrln("No test found with name: \"", argv[1], "\""), EXIT_FAILURE;
}
