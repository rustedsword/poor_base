#include <poor_array.h>
#include <signal.h>

static volatile sig_atomic_t argument_calls;
static volatile sig_atomic_t expected_calls;

static void exit_on_abort(int sig) {
	(void)sig;
	_Exit(argument_calls == expected_calls ? 0 : 1);
}

static int argument(void) {
	++argument_calls;
	return 42;
}

static void valid_views(size_t size) {
	int a[16] = {0};
	auto v = arrview(1, size, a);
	auto first = arrview_first(size, a);
	auto last = arrview_last(size, a);
	auto shrink = arrview_shrink(1, size, a);
	auto front = arrview_cfront(size, a);
	auto back = arrview_cback(size, a);
	auto dim = arrview_dim(size, a);

	assert(*v == a + 1 && ARRAY_SIZE(v) == size);
	assert(*first == a && ARRAY_SIZE(first) == size);
	assert(*last == a + 16 - size && ARRAY_SIZE(last) == size);
	assert(*shrink == a + 1 && ARRAY_SIZE(shrink) == 15 - size);
	assert(*front == a + size && ARRAY_SIZE(front) == 16 - size);
	assert(*back == a && ARRAY_SIZE(back) == 16 - size);
	assert(&(*dim)[0][0] == a && ARRAY_SIZE(dim) == 16 / size);
}

static int view_error(void) {
	int a[16] = {0};
	size_t i = 14;
#if POOR_ARRAY_CHECK == RUNTIME_CHECK
	fprintf(stdout, CRED "arrview(): Out of bound view  (start index:14 view size:3 source array size:16) at %s:%d" CRESET "\n",
		__FILE__, __LINE__ + 3);
	fflush(stdout);
#endif
	auto v = arrview(i, 3, a);
	assert(*v == a + 14 && ARRAY_SIZE(v) == 3);
	return POOR_ARRAY_CHECK == RUNTIME_CHECK;
}

static int formatted_error(void) {
	char text[] = "%s";
	fprintf(stdout, "100%% | %s | %jd | %ju | %c | %d | %d | %s | %d\n",
		text, INTMAX_MIN, UINTMAX_MAX, 'A', -7, 200, "true", 42);
	fflush(stdout);
	expected_calls = 1;
	auto result = arr_errmsg("100% | ", text, " | ", INTMAX_MIN, " | ", UINTMAX_MAX, " | ", (char)'A',
		" | ", (short)-7, " | ", (unsigned char)200, " | ", true, " | ", argument());
	return result + 1;
}

int main(int argc, char **argv) {
	assert(argc == 2);
	valid_views((size_t)argc);
	signal(SIGABRT, exit_on_abort);
	if(strcmp(argv[1], "view") == 0)
		return view_error();
	assert(strcmp(argv[1], "format") == 0);
	return formatted_error();
}
