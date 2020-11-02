#include <poor_array.h>

static int auto_arr_test(void) {
	unsigned char ar[2] = {3, 5}; //array
	unsigned char (*ar_p)[2] = &ar; //pointer to array

	unsigned char ar_v[(size_t){2}]; //vla
	ar_v[0] = 8; ar_v[1] = 13;
	unsigned char (*ar_v_p)[(size_t){2}] = &ar_v; //pointer to vla

	assert( auto_arr(ar)[0] == 3); //array
	assert( auto_arr(ar)[1] == 5);
	assert( auto_arr(&ar)[0] == 3); //addressof array
	assert( auto_arr(&ar)[1] == 5);

	assert( auto_arr(ar_p)[0] == 3); //pointer to array
	assert( auto_arr(ar_p)[1] == 5);
	assert( auto_arr(*ar_p)[0] == 3); //dereferenced pointer to array
	assert( auto_arr(*ar_p)[1] == 5);

	assert( auto_arr(ar_v)[0] == 8); //vla
	assert( auto_arr(ar_v)[1] == 13);
	assert( auto_arr(&ar_v)[0] == 8); //addressof vla
	assert( auto_arr(&ar_v)[1] == 13);

	assert( auto_arr(ar_v_p)[0] == 8); //pointer to vla
	assert( auto_arr(ar_v_p)[1] == 13);
	assert( auto_arr(*ar_v_p)[0] == 8); //dereferenced pointer to vla
	assert( auto_arr(*ar_v_p)[1] == 13);

	assert( auto_arr((int[]){4, 8})[1] == 8 ); //array as a compound literal
	assert( auto_arr(&(int[]){4, 8})[1] == 8 ); //addressof an array as a compound literal

	assert( auto_arr((unsigned char(*)[2]){&ar})[1] == 5 ); //pointer to array as a compound literal
	assert( auto_arr(*(unsigned char(*)[2]){&ar})[1] == 5 ); //dereferenced pointer to array as compound literal

	assert( auto_arr((unsigned char(*)[(size_t){2}]){&ar_v} )[1] == 13 ); //pointer to vla as a compound literal
	assert( auto_arr(*(unsigned char(*)[(size_t){2}]){&ar_v} )[1] == 13 ); //dereferenced pointer to array as a compound literal

	return 0;
}

static int array_size(void) {
	uint32_t a[4];
	static_assert( ARRAY_SIZE(a) == 4);
	static_assert( ARRAY_SIZE_BYTES(a) == 4 * sizeof(uint32_t));

	uint32_t b[(size_t){5}];
	assert( ARRAY_SIZE(b) == 5);
	assert( ARRAY_SIZE_BYTES(b) == 5 * sizeof(uint32_t));

	uint16_t (*c)[3];
	static_assert( ARRAY_SIZE(c) == 3);
	static_assert( ARRAY_SIZE_BYTES(c) == 3 * sizeof(uint16_t));

	uint64_t (*d)[(size_t){7}];
	assert( ARRAY_SIZE(d) == 7);
	assert( ARRAY_SIZE_BYTES(d) == 7 * sizeof(uint64_t));

	return 0;
}

static int arrays_size(void) {
	uint32_t a[4];
	uint32_t b[(size_t){5}];
	uint16_t (*c)[3];
	uint64_t (*d)[(size_t){7}];

	assert( ARRAYS_SIZE(a, b, c, d) == ARRAY_SIZE(a) + ARRAY_SIZE(b) + ARRAY_SIZE(c) + ARRAY_SIZE(d));
	assert( ARRAYS_SIZE_BYTES(a, b, c, d) == ARRAY_SIZE_BYTES(a) + ARRAY_SIZE_BYTES(b) + ARRAY_SIZE_BYTES(c) + ARRAY_SIZE_BYTES(d));

	static_assert(ARRAYS_SIZE(a, c) == ARRAY_SIZE(a) + ARRAY_SIZE(c));
	static_assert(ARRAYS_SIZE_BYTES(a, c) == ARRAY_SIZE_BYTES(a) + ARRAY_SIZE_BYTES(c));

	return 0;
}

static int fill_array_test(void) {
	{
		int a[8];
		fill_array(a, 34);
		foreach_array_ref(a, ref)
			assert(*ref == 34);
	}

	{
		int b[(size_t){7}];
		fill_array(b, 56);
		foreach_array_ref(b, ref)
			assert(*ref == 56);

	}

	{
		struct toto {
			int a;
			long b;
			bool c;
		};

		struct toto c[(size_t){13}];
		fill_array(c, (struct toto){.a = 1, .b = 3, .c = true});
		foreach_array_ref(c, ref) {
			assert(ref->a == 1);
			assert(ref->b == 3);
			assert(ref->c == true);
		}
	}
	return 0;
}

int array_accessors(void) {
	int a[] = {1,2,3,4,5,6,7,8,9,10};
	assert( *array_first_ref(a) == 1);
	assert( *array_last_ref(a) == 10);
	assert( array_end_ref(a) == &a[10]);

	assert( is_first_array_ref(a, &a[0]) == true);
	assert( is_last_array_ref(a, &a[9]) == true);
	assert( is_end_array_ref(a, &a[10]) == true);

	return 0;
}

int copy_array_single(void) {
	const int a[4] = {1,2,3,4};

	{	//t is smaller than a
		int t[3] = {0};
		copy_array(t, a);
		assert(t[0] == 1);
		assert(t[1] == 2);
		assert(t[2] == 3);
	}
	{	//t is larger than a
		int t[(size_t){5}];
		fill_array(t, 255);
		copy_array(t, a);
		assert(t[0] == 1);
		assert(t[1] == 2);
		assert(t[2] == 3);
		assert(t[3] == 4);
		assert(t[4] == 255);
	}
	{	//t is a pointer to array which is larger than a, and has compatible type
		long (*t)[5] = &(long[5]){0};
		fill_array(t, 255);
		copy_array(t, a);
		assert(auto_arr(t)[0] == 1);
		assert(auto_arr(t)[1] == 2);
		assert(auto_arr(t)[2] == 3);
		assert(auto_arr(t)[3] == 4);
		assert(auto_arr(t)[4] == 255);
	}
	return 0;
}

int copy_array_multiple(void) {
	{
		int a[] = {1,2};
		int b[] = {5,6,7};

		//create destination array with 1 more element than source arrays a and b and copy data there
		int t[ARRAYS_SIZE(a, b) + 1];
		static_assert(sizeof(t) == sizeof(a) + sizeof (b) + sizeof(int));
		fill_array(t, 99);
		copy_arrays(t, a, b);

		assert(t[0] == 1);
		assert(t[1] == 2);

		assert(t[2] == 5);
		assert(t[3] == 6);
		assert(t[4] == 7);

		assert(t[5] == 99);
	}
	{
		int a[2][2] = {{1,2},{3,4}};
		int b[1][2] = {{5,6}};

		int t[4][2] = {0};
		copy_arrays(t, a, b);
		assert(t[0][0] == 1);
		assert(t[0][1] == 2);
		assert(t[1][0] == 3);
		assert(t[1][1] == 4);
		assert(t[2][0] == 5);
		assert(t[2][1] == 6);
		assert(t[3][0] == 0);
		assert(t[3][1] == 0);
	}


	return 0;
}
typedef int test_fn (void);

#define TEST_FN(fn) {STRINGIFY2(fn), fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(auto_arr_test),
	TEST_FN(array_size),
	TEST_FN(arrays_size),
	TEST_FN(fill_array_test),
	TEST_FN(array_accessors),
	TEST_FN(copy_array_single),
	TEST_FN(copy_array_multiple),
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
