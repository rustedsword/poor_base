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

typedef int test_fn (void);

#define TEST_FN(fn) {STRINGIFY2(fn), fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(auto_arr_test),
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
