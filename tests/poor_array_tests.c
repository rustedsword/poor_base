#include <poor_array.h>
#include <stdio.h>
#include <assert.h>

#undef NDEBUG

static int auto_arr_test(void) {
	unsigned char ar[2] = {3, 5}; //array
	unsigned char (*ar_p)[2] = &ar; //pointer to array

	unsigned char ar_v[(size_t){2}]; //vla
	ar_v[0] = 8; ar_v[1] = 13;
	unsigned char (*ar_v_p)[(size_t){2}] = &ar_v; //pointer to vla

	(void)ar, (void)ar_p, (void)ar_v, (void)ar_v_p;

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

	//string literals are arrays, so auto_arr() accepts them
	assert( auto_arr("desu")[0] == 'd' );
	assert( auto_arr("desu")[4] == '\0' );
	static_assert( ARRAY_SIZE(auto_arr("desu")) == 5 );
	assert( auto_arr(&"desu")[1] == 'e' ); //pointer to the literal's array

	assert( auto_arr((int[]){4, 8})[1] == 8 ); //array as a compound literal
	assert( auto_arr(&(int[]){4, 8})[1] == 8 ); //addressof an array as a compound literal

	assert( auto_arr((unsigned char(*)[2]){&ar})[1] == 5 ); //pointer to array as a compound literal
	assert( auto_arr(*(unsigned char(*)[2]){&ar})[1] == 5 ); //dereferenced pointer to array as compound literal

	assert( auto_arr((unsigned char(*)[(size_t){2}]){&ar_v} )[1] == 13 ); //pointer to vla as a compound literal
	assert( auto_arr(*(unsigned char(*)[(size_t){2}]){&ar_v} )[1] == 13 ); //dereferenced pointer to array as a compound literal

	//multi-dimensional array and pointer to array
	int md_v0[2][1] = {{1},{2}};
	int (*pv0)[2][1] = &md_v0;
	assert(auto_arr(md_v0)[0][0] == 1);
	assert(auto_arr(md_v0)[1][0] == 2);
	assert(auto_arr(pv0)[0][0] == 1);
	assert(auto_arr(pv0)[1][0] == 2);

	//first dimension is variable, second is static
	typedef int md_v1_t[(size_t){2}][1];
	md_v1_t md_v1, *pv1 = &md_v1;
	md_v1[0][0] = 1; md_v1[1][0] = 2;

	assert(auto_arr(md_v1)[0][0] == 1);
	assert(auto_arr(md_v1)[1][0] == 2);
	assert(auto_arr(pv1)[0][0] == 1);
	assert(auto_arr(pv1)[1][0] == 2);

	//first dimension is static, second is variable
	typedef int md_v2_t[2][(size_t){1}];
	md_v2_t md_v2, *pv2 = &md_v2;
	md_v2[0][0] = 1; md_v2[1][0] = 2;

	assert(auto_arr(md_v2)[0][0] == 1);
	assert(auto_arr(md_v2)[1][0] == 2);
	assert(auto_arr(pv2)[0][0] == 1);
	assert(auto_arr(pv2)[1][0] == 2);

	//2 dimensional VLA
	typedef int md_v3_t[(size_t){2}][(size_t){1}];
	md_v3_t md_v3, *pv3 = &md_v3;
	md_v3[0][0] = 1; md_v3[1][0] = 2;

	assert(auto_arr(md_v3)[0][0] == 1);
	assert(auto_arr(md_v3)[1][0] == 2);
	assert(auto_arr(pv3)[0][0] == 1);
	assert(auto_arr(pv3)[1][0] == 2);

	(void)md_v0, (void)pv0;
	(void)md_v1, (void)pv1;
	(void)md_v2, (void)pv2;
	(void)md_v3, (void)pv3;

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

	(void)a, (void)b, (void)c, (void)d;
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

	(void)a, (void)b, (void)c, (void)d;
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

/* Stop immediately if an empty iterator enters its body, even with a bad ref. */
#define CHECK_EMPTY_ITERATOR(iterator, input) do { \
	size_t visited = 0; \
	iterator(input, ref) { \
		(void)ref; \
		visited++; \
		break; \
	} \
	assert(visited == 0); \
} while (0)

/* Fixed [0] arrays use the GNU extension, not a VLA with a runtime zero bound.
 * For these fixed types, reference iterators evaluate their input once; the
 * index iterator only queries its type with sizeof and does not evaluate it. */
#define DEFINE_EMPTY_ITERATOR_TESTS(iterator, evaluations_expected) \
static int iterator##_zero_length_test(void) { \
	int empty[0]; \
	int (*empty_ptr)[0] = &empty; \
	int storage[] = {11, 13}; \
	const int (*const_empty_ptr)[0] = (const int (*)[0])storage; \
	static_assert(ARRAY_SIZE(empty_ptr) == 0); \
	CHECK_EMPTY_ITERATOR(iterator, empty); \
	CHECK_EMPTY_ITERATOR(iterator, &empty); \
	CHECK_EMPTY_ITERATOR(iterator, empty_ptr); \
	CHECK_EMPTY_ITERATOR(iterator, *empty_ptr); \
	CHECK_EMPTY_ITERATOR(iterator, (int (*)[0])storage); \
	CHECK_EMPTY_ITERATOR(iterator, const_empty_ptr); \
	int evaluations = 0; \
	CHECK_EMPTY_ITERATOR(iterator, (evaluations++, empty_ptr)); \
	assert(evaluations == evaluations_expected); \
	assert(storage[0] == 11 && storage[1] == 13); \
	return 0; \
} \
static int iterator##_null_zero_length_test(void) { \
	int (*null_ptr)[0] = NULL; \
	const int (*const_null_ptr)[0] = NULL; \
	CHECK_EMPTY_ITERATOR(iterator, null_ptr); \
	CHECK_EMPTY_ITERATOR(iterator, const_null_ptr); \
	CHECK_EMPTY_ITERATOR(iterator, (int (*)[0])NULL); \
	CHECK_EMPTY_ITERATOR(iterator, (const int (*)[0])NULL); \
	int evaluations = 0; \
	CHECK_EMPTY_ITERATOR(iterator, (evaluations++, null_ptr)); \
	assert(evaluations == evaluations_expected); \
	return 0; \
}

DEFINE_EMPTY_ITERATOR_TESTS(foreach_array_ref, 1)
DEFINE_EMPTY_ITERATOR_TESTS(foreach_array_const_ref, 1)
DEFINE_EMPTY_ITERATOR_TESTS(foreach_array_ref_bw, 1)
DEFINE_EMPTY_ITERATOR_TESTS(foreach_array_const_ref_bw, 1)
DEFINE_EMPTY_ITERATOR_TESTS(foreach_array_index, 0)
DEFINE_EMPTY_ITERATOR_TESTS(foreach_array_index_bw, 0)

#undef DEFINE_EMPTY_ITERATOR_TESTS
#undef CHECK_EMPTY_ITERATOR

static int foreach_array_ref_test(void) {
	int fixed[] = {2, 3, 5};
	size_t visited = 0;
	foreach_array_ref(&fixed, ref) {
		static_assert(_Generic(ref, int *: true, default: false));
		assert(visited < ARRAY_SIZE(fixed));
		assert(ref == &fixed[visited++]);
	}
	assert(visited == ARRAY_SIZE(fixed));

	visited = 0;
	foreach_array_const_ref(fixed, ref) {
		static_assert(_Generic(ref, const int *: true, default: false));
		assert(visited < ARRAY_SIZE(fixed));
		assert(ref == &fixed[visited++]);
	}
	assert(visited == ARRAY_SIZE(fixed));

	visited = 0;
	foreach_array_ref_bw(&fixed, ref) {
		static_assert(_Generic(ref, int *: true, default: false));
		assert(visited < ARRAY_SIZE(fixed));
		assert(ref == &fixed[ARRAY_SIZE(fixed) - 1 - visited++]);
		*ref += 1;
	}
	assert(visited == ARRAY_SIZE(fixed));
	assert(fixed[0] == 3 && fixed[1] == 4 && fixed[2] == 6);

	visited = 0;
	foreach_array_const_ref_bw(fixed, ref) {
		static_assert(_Generic(ref, const int *: true, default: false));
		assert(visited < ARRAY_SIZE(fixed));
		assert(ref == &fixed[ARRAY_SIZE(fixed) - 1 - visited++]);
	}
	assert(visited == ARRAY_SIZE(fixed));

	const int singleton[] = {7};
	visited = 0;
	foreach_array_ref_bw(&singleton, ref) {
		static_assert(_Generic(ref, const int *: true, default: false));
		assert(ref == &singleton[0]);
		assert(*ref == 7);
		visited++;
	}
	assert(visited == 1);

	size_t length = 3;
	int vla[length];
	for(size_t i = 0; i < length; i++)
		vla[i] = (int)i;
	visited = 0;
	int sum = 0;
	foreach_array_ref_bw(&vla, ref) {
		assert(visited < length);
		assert(ref == &vla[length - 1 - visited++]);
		if(*ref == 1)
			continue;
		sum += *ref;
	}
	assert(visited == length);
	assert(sum == 2);

	visited = 0;
	foreach_array_const_ref_bw(&vla, ref) {
		assert(ref == &vla[length - 1]);
		visited++;
		break;
	}
	assert(visited == 1);

	int matrix[2][3] = {{1, 2, 3}, {4, 5, 6}};
	visited = 0;
	foreach_array_ref_bw(matrix, row)
		foreach_array_ref_bw(*row, ref) {
			assert(*ref == 6 - (int)visited);
			visited++;
		}
	assert(visited == 6);

	return 0;
}

static int foreach_array_index_test(void) {
	int fixed[] = {2, 3, 5, 7};
	size_t visited = 0;
	int sum = 0;
	foreach_array_index(fixed, index) {
		static_assert(_Generic(index, size_t: true, default: false));
		assert(index == visited++);
		sum += fixed[index];
	}
	assert(visited == ARRAY_SIZE(fixed));
	assert(sum == 17);

	int (*fixed_ptr)[ARRAY_SIZE(fixed)] = &fixed;
	foreach_array_index(fixed_ptr, index)
		assert(auto_arr(fixed_ptr)[index] == fixed[index]);

	size_t vla_size = 3;
	int vla[vla_size];
	int (*vla_ptr)[vla_size] = &vla;
	visited = 0;
	foreach_array_index(vla_ptr, index) {
		vla[index] = (int)index;
		visited++;
	}
	assert(visited == vla_size);

	int matrix[2][3] = {0};
	visited = 0;
	foreach_array_index(matrix, row)
		foreach_array_index(matrix[row], column) {
			matrix[row][column] = 1;
			visited++;
		}
	assert(visited == 6);

	visited = 0;
	foreach_array_index(((int[]){11, 13, 17}), index) {
		assert(index == visited);
		visited++;
	}
	assert(visited == 3);

	int evaluations = 0;
	foreach_array_index((evaluations++, vla_ptr), index)
		(void)index;
	assert(evaluations == 1);

	return 0;
}

static int foreach_array_index_bw_test(void) {
	int fixed[] = {2, 3, 5, 7};
	size_t visited = 0;
	int sum = 0;
	foreach_array_index_bw(fixed, index) {
		static_assert(_Generic(index, size_t: true, default: false));
		assert(index == ARRAY_SIZE(fixed) - 1 - visited++);
		if(index == 2)
			continue;
		sum += fixed[index];
	}
	assert(visited == ARRAY_SIZE(fixed));
	assert(sum == 12);

	int (*fixed_ptr)[ARRAY_SIZE(fixed)] = &fixed;
	foreach_array_index_bw(fixed_ptr, index)
		assert(auto_arr(fixed_ptr)[index] == fixed[index]);

	size_t vla_size = 3;
	int vla[vla_size];
	int (*vla_ptr)[vla_size] = &vla;
	visited = 0;
	foreach_array_index_bw(vla_ptr, index) {
		vla[index] = (int)index;
		visited++;
	}
	assert(visited == vla_size);

	int matrix[2][3] = {0};
	visited = 0;
	foreach_array_index_bw(matrix, row)
		foreach_array_index_bw(matrix[row], column) {
			assert(row == 1 - visited / 3);
			assert(column == 2 - visited % 3);
			visited++;
		}
	assert(visited == 6);

	int evaluations = 0;
	foreach_array_index_bw((evaluations++, vla_ptr), index)
		(void)index;
	assert(evaluations == 1);

	return 0;
}

static int array_accessors(void) {
	int a[] = {1,2,3,4,5,6,7,8,9,10};
	assert( *array_first_ref(a) == 1);
	assert( *array_last_ref(a) == 10);
	assert( array_end_ref(a) == &a[10]);

	assert( is_first_array_ref(a, &a[0]) == true);
	assert( is_last_array_ref(a, &a[9]) == true);
	assert( is_end_array_ref(a, &a[10]) == true);

	return 0;
}

static int copy_array_single(void) {
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
	{	//multi-dimensional copy
		int t1[2][3] = {{1,2,3}, {4,5,6}};
		int t2[1][3] = {{7,8,9}};
		copy_array(t1, t2);
		assert(t1[0][0] == 7);
		assert(t1[0][1] == 8);
		assert(t1[0][2] == 9);
		assert(t1[1][0] == 4);
		assert(t1[1][1] == 5);
		assert(t1[1][2] == 6);
	}
	return 0;
}

static int copy_array_multiple(void) {
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
	{	//multi-dimensional copy
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

static int merged_array_test(void) {
	const int a[] = {1,2,3};
	int b[(size_t){2}];
	copy_array(b, (int[]){4,5});

	int (*c)[2] = &(int[]){6,7};
	int (*d)[(size_t){1}] = &(int[]){8};

	make_merged_array(merged, a, b, c, d);
	assert(ARRAYS_SIZE(a,b,c,d) == ARRAY_SIZE(merged));

	assert(arr(merged)[0] == 1);
	assert(arr(merged)[1] == 2);
	assert(arr(merged)[2] == 3);
	assert(arr(merged)[3] == 4);
	assert(arr(merged)[4] == 5);
	assert(arr(merged)[5] == 6);
	assert(arr(merged)[6] == 7);
	assert(arr(merged)[7] == 8);

	//multi-dimensional merge, needs typeof_unqual() to build the element type
	int md_a[2][2] = {{1,2},{3,4}};
	int md_b[1][2] = {{5,6}};
	make_merged_array(md, md_a, md_b);

	static_assert(ARRAY_SIZE(md) == ARRAY_SIZE(md_a) + ARRAY_SIZE(md_b));
	static_assert(ARRAY_SIZE(arr(md)[0]) == 2);
	static_assert(ARRAY_SIZE_BYTES(md) == ARRAY_SIZE_BYTES(md_a) + ARRAY_SIZE_BYTES(md_b));

	assert(arr(md)[0][0] == 1 && arr(md)[0][1] == 2);
	assert(arr(md)[1][0] == 3 && arr(md)[1][1] == 4);
	assert(arr(md)[2][0] == 5 && arr(md)[2][1] == 6);

	//element type must come out unqualified
	const int md_c[1][2] = {{7,8}};
	make_merged_array(md2, md_c, md_b);
	arr(md2)[0][0] = 70;
	assert(arr(md2)[0][0] == 70 && arr(md2)[1][1] == 6);

	return 0;
}

static int same_type_arrays(void) {
	int i1[1];
	const int i1c[1] = {0};
	int (*i1p)[1] = &i1;
	const int (*i1cp)[1] = &i1c;
	int (*const ci1p)[1] = &i1;
	const int (*const ci1cp)[1] = &i1c;

	int i2[2];
	const int i2c[2] = {0};
	int (*i2p)[2] = &i2;
	const int (*i2cp)[2] = &i2c;
	int (*const ci2p)[2] = &i2;
	const int (*const ci2cp)[2] = &i2c;

	short s1[1];
	const short s1c[1] = {0};
	short (*s1p)[1] = &s1;
	const short (*s1cp)[1] = &s1c;
	short (*const cs1p)[1] = &s1;
	const short (*const cs1cp)[1] = &s1c;

	static_assert(is_same_array_element_type(i1, i1c) == true);
	static_assert(is_same_array_element_type(i1, i1p) == true);
	static_assert(is_same_array_element_type(i1, i1cp) == true);
	static_assert(is_same_array_element_type(i1, ci1p) == true);
	static_assert(is_same_array_element_type(i1, ci1cp) == true);

	static_assert(is_same_array_element_type(i1c, i1c) == true);
	static_assert(is_same_array_element_type(i1c, i1p) == true);
	static_assert(is_same_array_element_type(i1c, i1cp) == true);
	static_assert(is_same_array_element_type(i1c, ci1p) == true);
	static_assert(is_same_array_element_type(i1c, ci1cp) == true);

	static_assert(is_same_array_element_type(i1p, i1c) == true);
	static_assert(is_same_array_element_type(i1p, i1p) == true);
	static_assert(is_same_array_element_type(i1p, i1cp) == true);
	static_assert(is_same_array_element_type(i1p, ci1p) == true);
	static_assert(is_same_array_element_type(i1p, ci1cp) == true);

	static_assert(is_same_array_element_type(i1cp, i1c) == true);
	static_assert(is_same_array_element_type(i1cp, i1p) == true);
	static_assert(is_same_array_element_type(i1cp, i1cp) == true);
	static_assert(is_same_array_element_type(i1cp, ci1p) == true);
	static_assert(is_same_array_element_type(i1cp, ci1cp) == true);

	static_assert(is_same_array_element_type(ci1p, i1c) == true);
	static_assert(is_same_array_element_type(ci1p, i1p) == true);
	static_assert(is_same_array_element_type(ci1p, i1cp) == true);
	static_assert(is_same_array_element_type(ci1p, ci1p) == true);
	static_assert(is_same_array_element_type(ci1p, ci1cp) == true);

	static_assert(is_same_array_element_type(ci1cp, i1c) == true);
	static_assert(is_same_array_element_type(ci1cp, i1p) == true);
	static_assert(is_same_array_element_type(ci1cp, i1cp) == true);
	static_assert(is_same_array_element_type(ci1cp, ci1p) == true);
	static_assert(is_same_array_element_type(ci1cp, ci1cp) == true);

	/* int[1] vs int[2] */
	static_assert(is_same_array_element_type(i2, i1c) == true);
	static_assert(is_same_array_element_type(i2, i1p) == true);
	static_assert(is_same_array_element_type(i2, i1cp) == true);
	static_assert(is_same_array_element_type(i2, ci1p) == true);
	static_assert(is_same_array_element_type(i2, ci1cp) == true);

	static_assert(is_same_array_element_type(i2c, i1c) == true);
	static_assert(is_same_array_element_type(i2c, i1p) == true);
	static_assert(is_same_array_element_type(i2c, i1cp) == true);
	static_assert(is_same_array_element_type(i2c, ci1p) == true);
	static_assert(is_same_array_element_type(i2c, ci1cp) == true);

	static_assert(is_same_array_element_type(i2p, i1c) == true);
	static_assert(is_same_array_element_type(i2p, i1p) == true);
	static_assert(is_same_array_element_type(i2p, i1cp) == true);
	static_assert(is_same_array_element_type(i2p, ci1p) == true);
	static_assert(is_same_array_element_type(i2p, ci1cp) == true);

	static_assert(is_same_array_element_type(i2cp, i1c) == true);
	static_assert(is_same_array_element_type(i2cp, i1p) == true);
	static_assert(is_same_array_element_type(i2cp, i1cp) == true);
	static_assert(is_same_array_element_type(i2cp, ci1p) == true);
	static_assert(is_same_array_element_type(i2cp, ci1cp) == true);

	static_assert(is_same_array_element_type(ci2p, i1c) == true);
	static_assert(is_same_array_element_type(ci2p, i1p) == true);
	static_assert(is_same_array_element_type(ci2p, i1cp) == true);
	static_assert(is_same_array_element_type(ci2p, ci1p) == true);
	static_assert(is_same_array_element_type(ci2p, ci1cp) == true);

	static_assert(is_same_array_element_type(ci2cp, i1c) == true);
	static_assert(is_same_array_element_type(ci2cp, i1p) == true);
	static_assert(is_same_array_element_type(ci2cp, i1cp) == true);
	static_assert(is_same_array_element_type(ci2cp, ci1p) == true);
	static_assert(is_same_array_element_type(ci2cp, ci1cp) == true);

	/* int vs short */
	static_assert(is_same_array_element_type(s1, i1c) == false);
	static_assert(is_same_array_element_type(s1, i1p) == false);
	static_assert(is_same_array_element_type(s1, i1cp) == false);
	static_assert(is_same_array_element_type(s1, ci1p) == false);
	static_assert(is_same_array_element_type(s1, ci1cp) == false);

	static_assert(is_same_array_element_type(s1c, i1c) == false);
	static_assert(is_same_array_element_type(s1c, i1p) == false);
	static_assert(is_same_array_element_type(s1c, i1cp) == false);
	static_assert(is_same_array_element_type(s1c, ci1p) == false);
	static_assert(is_same_array_element_type(s1c, ci1cp) == false);

	static_assert(is_same_array_element_type(s1p, i1c) == false);
	static_assert(is_same_array_element_type(s1p, i1p) == false);
	static_assert(is_same_array_element_type(s1p, i1cp) == false);
	static_assert(is_same_array_element_type(s1p, ci1p) == false);
	static_assert(is_same_array_element_type(s1p, ci1cp) == false);

	static_assert(is_same_array_element_type(s1cp, i1c) == false);
	static_assert(is_same_array_element_type(s1cp, i1p) == false);
	static_assert(is_same_array_element_type(s1cp, i1cp) == false);
	static_assert(is_same_array_element_type(s1cp, ci1p) == false);
	static_assert(is_same_array_element_type(s1cp, ci1cp) == false);

	static_assert(is_same_array_element_type(cs1p, i1c) == false);
	static_assert(is_same_array_element_type(cs1p, i1p) == false);
	static_assert(is_same_array_element_type(cs1p, i1cp) == false);
	static_assert(is_same_array_element_type(cs1p, ci1p) == false);
	static_assert(is_same_array_element_type(cs1p, ci1cp) == false);

	static_assert(is_same_array_element_type(cs1cp, i1c) == false);
	static_assert(is_same_array_element_type(cs1cp, i1p) == false);
	static_assert(is_same_array_element_type(cs1cp, i1cp) == false);
	static_assert(is_same_array_element_type(cs1cp, ci1p) == false);
	static_assert(is_same_array_element_type(cs1cp, ci1cp) == false);

	//all qualifiers are ignored, not just const
	volatile int v1[1] = {0};
	const volatile int cv1[1] = {0};
	_Atomic int a1[1];

	static_assert(is_same_array_element_type(i1, v1) == true);
	static_assert(is_same_array_element_type(v1, i1) == true);
	static_assert(is_same_array_element_type(i1, cv1) == true);
	static_assert(is_same_array_element_type(i1c, v1) == true);
	static_assert(is_same_array_element_type(i1, a1) == true);
	static_assert(is_same_array_element_type(a1, i1c) == true);

	//element type still has to match
	static_assert(is_same_array_element_type(s1, v1) == false);
	static_assert(is_same_array_element_type(s1, a1) == false);

	/* only the outermost size is ignored: for a multi-dimensional array the
	 * element type is an array itself, so inner dimensions must match */
	int d23[2][3], e53[5][3], f24[2][4];
	const int cd23[2][3] = {{0}};

	static_assert(is_same_array_element_type(d23, e53) == true);
	static_assert(is_same_array_element_type(d23, f24) == false);
	static_assert(is_same_array_element_type(cd23, d23) == true);

	(void)v1, (void)cv1, (void)d23, (void)e53, (void)f24;
	return 0;
}

static int arrview_simple(void) {
	//arrview from array
	int a[] = {1,2,3,4,5};
	make_arrview(a_view, 1, 3, a);
	static_assert(ARRAY_SIZE(a_view) == 3);
	assert(auto_arr(a_view)[0] == 2);
	assert(auto_arr(a_view)[1] == 3);
	assert(auto_arr(a_view)[2] == 4);

	//view that takes whole array should be equal to original array
	make_arrview(a_view_1, 0, 5, a);
	static_assert(ARRAYS_SIZE(a_view_1) == ARRAY_SIZE(a));
	assert(a_view_1 == &a);

	//arrview from VLA
	int b[(size_t){5}];
	copy_array(b, (int[]){1,2,3,4,5});
	make_arrview(b_view, 2, 2, b);
	assert(ARRAY_SIZE(b_view) == 2);
	assert(auto_arr(b_view)[0] == 3);
	assert(auto_arr(b_view)[1] == 4);

	return 0;
}

static int arrview_first_test(void) {
	int a[] = {1,2,3,4,5};
	//take first 3 elements
	make_arrview_first(a_view, 3, a);
	static_assert(ARRAY_SIZE(a_view) == 3);
	assert(auto_arr(a_view)[0] == 1);
	assert(auto_arr(a_view)[1] == 2);
	assert(auto_arr(a_view)[2] == 3);

	//view that cut all elements should be equal to original array
	make_arrview_first(a_view_2, 5, a);
	static_assert(ARRAY_SIZE(a_view_2) == ARRAY_SIZE(a));

	return 0;
}

static int arrview_last_test(void) {
	int a[] = {1,2,3,4,5};
	//take last 3 elements
	make_arrview_last(a_view, 3, a);
	static_assert(ARRAY_SIZE(a_view) == 3);
	assert(auto_arr(a_view)[0] == 3);
	assert(auto_arr(a_view)[1] == 4);
	assert(auto_arr(a_view)[2] == 5);

	//view that cut all elements should be equal to original array
	make_arrview_last(a_view_2, 5, a);
	static_assert(ARRAY_SIZE(a_view_2) == ARRAY_SIZE(a));

	return 0;
}

static int arrview_shrink_test(void) {
	int a[] = {1,2, 3,4, 5,6,7};

	//view without first two and last 3 elements
	make_arrview_shrink(a_view_1, 2, 3, a);
	static_assert(ARRAY_SIZE(a_view_1) == ARRAY_SIZE(a) - 2 - 3);
	assert(auto_arr(a_view_1)[0] == 3);
	assert(auto_arr(a_view_1)[1] == 4);

	//view without first two elements
	make_arrview_cfront(a_view_2, 2, a);
	static_assert(ARRAY_SIZE(a_view_2) == ARRAY_SIZE(a) - 2);
	assert(auto_arr(a_view_2)[0] == 3);
	assert(auto_arr(a_view_2)[1] == 4);
	assert(auto_arr(a_view_2)[2] == 5);
	assert(auto_arr(a_view_2)[3] == 6);
	assert(auto_arr(a_view_2)[4] == 7);

	//view without last 4 elements
	make_arrview_cback(a_view_3, 4, a);
	static_assert(ARRAY_SIZE(a_view_3) == ARRAY_SIZE(a) - 4);
	assert(auto_arr(a_view_3)[0] == 1);
	assert(auto_arr(a_view_3)[1] == 2);
	assert(auto_arr(a_view_3)[2] == 3);

	//views that do not cut anything should be equal to original array
	make_arrview_shrink(a_view_t1, 0, 0, a);
	make_arrview_cback(a_view_t2, 0, a);
	make_arrview_cfront(a_view_t3, 0, a);

	assert(ARRAY_SIZE(a_view_t1) == ARRAY_SIZE(a) );
	assert(a_view_t1 == &a);
	assert(ARRAY_SIZE(a_view_t2) == ARRAY_SIZE(a) );
	assert(a_view_t2 == &a);
	assert(ARRAY_SIZE(a_view_t3) == ARRAY_SIZE(a) );
	assert(a_view_t2 == &a);
	return 0;
}

static int array_dim_flat_test(void) {
	int a[] = {1,2,3,4,5,6,7,8,9,10};

	make_arrview_dim(a_pair, 2, a);
	static_assert(ARRAY_SIZE(a) == ARRAY_SIZE(a_pair) * ARRAY_SIZE(**a_pair));
	assert(auto_arr(a)[0] == auto_arr(a_pair)[0][0]);
	assert(auto_arr(a)[9] == auto_arr(a_pair)[4][1]);

	make_arrview_dim(a_all, 10, a);
	static_assert(ARRAY_SIZE(a) == ARRAY_SIZE(a_all) * ARRAY_SIZE(**a_all));
	assert(auto_arr(a)[0] == auto_arr(a_all)[0][0]);
	assert(auto_arr(a)[9] == auto_arr(a_all)[0][9]);

	make_arrview_dim(a_uneven, 3, a);
	static_assert(ARRAY_SIZE(a) == 1 + ARRAY_SIZE(a_uneven) * ARRAY_SIZE(**a_uneven));
	assert(auto_arr(a)[0] == auto_arr(a_uneven)[0][0]);
	assert(auto_arr(a)[8] == auto_arr(a_uneven)[2][2]);

	make_arrview_flat(a2, a_pair);
	static_assert(ARRAY_SIZE(a2) == ARRAY_SIZE(a));
	assert(&a == a2);

	make_arrview_flat(a2_u, a_uneven);
	static_assert(ARRAY_SIZE(a2_u) + 1 == ARRAY_SIZE(a));

	return 0;
}

static string_literal(file_scope_str, "I am just a string");

#define EXPORTED_LIT "Exported"
extern declare_string_literal(exported_str, EXPORTED_LIT);
string_literal(exported_str, EXPORTED_LIT);

static int string_literal_test(void) {
	string_literal(str, "String Literal");

	//size includes the terminating '\0'
	static_assert(ARRAY_SIZE(str) == sizeof("String Literal"));
	static_assert(ARRAY_SIZE(str) == 15);
	static_assert(ARRAY_ELEMENT_SIZE(str) == 1);

	//chars are const, so the literal cannot be written through
	static_assert(is_pointer_to_const(*str) == true);

	assert(auto_arr(str)[0] == 'S');
	assert(auto_arr(str)[13] == 'l');
	assert(auto_arr(str)[14] == '\0');
	assert(!strcmp(*str, "String Literal"));

	//a string view drops the '\0'
	make_arrview_str(str_nonull, str);
	static_assert(ARRAY_SIZE(str_nonull) == ARRAY_SIZE(str) - 1);
	assert(auto_arr(str_nonull)[13] == 'l');

	auto str_nonull_auto = arrview_str(str);
	static_assert(is_same_array_element_type(str_nonull, str_nonull_auto) == true);
	assert(str_nonull == str_nonull_auto);

	//declared at file scope with static
	static_assert(ARRAY_SIZE(file_scope_str) == sizeof("I am just a string"));
	assert(!strcmp(*file_scope_str, "I am just a string"));

	//declared with extern declare_string_literal(), defined with string_literal()
	static_assert(ARRAY_SIZE(exported_str) == sizeof(EXPORTED_LIT));
	assert(!strcmp(*exported_str, EXPORTED_LIT));

	//behaves like any other pointer to array
	size_t n = 0;
	foreach_array_ref(str, ref) {
		assert(*ref == auto_arr(str)[n]);
		n++;
	}
	assert(n == ARRAY_SIZE(str));

	return 0;
}

static int array_ptr_test(void) {
	//omitting the size wraps a single object without copying it
	int lol = 5;
	int (*lolptr)[1];
	lolptr = array_ptr(&lol);
	static_assert(ARRAY_SIZE(array_ptr(&lol)) == 1);
	assert(auto_arr(lolptr)[0] == 5);
	auto_arr(lolptr)[0] = 10;
	assert(lol == 10);
	lolptr = array_ptr(&lol, 1);
	assert(lolptr == array_ptr(&lol));

	//the default size preserves const and evaluates an ordinary pointer once
	const int value = 7;
	const int *value_ptr = &value;
	auto single = array_ptr(value_ptr++);
	static_assert(ARRAY_SIZE(single) == 1);
	static_assert(is_pointer_to_const(*single) == true);
	assert(auto_arr(single)[0] == 7);
	assert(value_ptr == &value + 1);

	const char *n = "string";

	auto a = array_ptr(n, 6);
	static_assert(ARRAY_SIZE(a) == 6);
	static_assert(sizeof(*a) == 6);
	static_assert(is_pointer_to_const(*a) == true);
	assert(auto_arr(a)[0] == 's' && auto_arr(a)[5] == 'g');

	//expression form and declaring form must agree on type and value
	make_array_ptr(m, n, 6);
	static_assert(is_same_array_element_type(a, m) == true);
	assert(a == m);

	//non const pointer stays non const and aliases the original storage
	int raw[8] = {0,1,2,3,4,5,6,7};
	auto p = array_ptr(raw, 4);
	static_assert(ARRAY_SIZE(p) == 4);
	static_assert(is_pointer_to_const(*p) == false);
	assert(auto_arr(p)[3] == 3);
	auto_arr(p)[3] = 30;
	assert(raw[3] == 30);

	//initializer commas introduced by a size macro are not extra arguments
#define ARRAY_PTR_TEST_BOUND sizeof (char[]){1, 2, 3}
	auto macro_sized = array_ptr(raw, ARRAY_PTR_TEST_BOUND);
	static_assert(ARRAY_SIZE(macro_sized) == 3);
	assert(auto_arr(macro_sized)[2] == 2);

	//size arguments may themselves contain array_ptr() calls
	auto nested_size = array_ptr(raw, ARRAY_SIZE(array_ptr(raw, ARRAY_PTR_TEST_BOUND)));
	static_assert(ARRAY_SIZE(nested_size) == 3);
	assert(nested_size == macro_sized);
#undef ARRAY_PTR_TEST_BOUND

	//a size macro may also expand to a comma expression with side effects
	int size_evaluations = 0;
#define ARRAY_PTR_TEST_COMMA_BOUND ++size_evaluations, 3
	auto comma_sized = array_ptr(raw, ARRAY_PTR_TEST_COMMA_BOUND);
#undef ARRAY_PTR_TEST_COMMA_BOUND
	assert(size_evaluations == 1);
	assert(ARRAY_SIZE(comma_sized) == 3);
	assert(comma_sized == macro_sized);

	//the result is a normal pointer to array, usable by the rest of the library
	auto v = arrview_last(2, p);
	static_assert(ARRAY_SIZE(v) == 2);
	assert(auto_arr(v)[1] == 30);

	//runtime size produces a pointer to VLA
	size_t len = 5;
	auto r = array_ptr(raw, len);
	assert(ARRAY_SIZE(r) == 5);
	assert(is_ptr_to_vla(r) == true);
	assert(auto_arr(r)[4] == 4);

	/* the size argument must accept any integer type, not a fixed list of them:
	 * a _Generic based check here would wrongly reject _BitInt and __int128 */
	char c_sz = 3;                 assert(ARRAY_SIZE(array_ptr(raw, c_sz)) == 3);
	short s_sz = 4;                assert(ARRAY_SIZE(array_ptr(raw, s_sz)) == 4);
	unsigned long long ull_sz = 6; assert(ARRAY_SIZE(array_ptr(raw, ull_sz)) == 6);
	size_t st_sz = 7;              assert(ARRAY_SIZE(array_ptr(raw, st_sz)) == 7);
	enum { K = 2 };                static_assert(ARRAY_SIZE(array_ptr(raw, K)) == 2);

#ifdef __BITINT_MAXWIDTH__
	_BitInt(16) bi_sz = 5;         assert(ARRAY_SIZE(array_ptr(raw, bi_sz)) == 5);
#endif
#ifdef __SIZEOF_INT128__
	__int128 i128_sz = 8;          assert(ARRAY_SIZE(array_ptr(raw, i128_sz)) == 8);
#endif

	return 0;
}

/* every arrview expression form must survive C23 auto with its
 * pointer-to-array type, size and constness intact */
static int arrview_auto_test(void) {
	int a[] = {1,2,3,4,5};

	auto v = arrview(1, 3, a);
	static_assert(ARRAY_SIZE(v) == 3);
	static_assert(sizeof(*v) == 3 * sizeof(int));
	assert(auto_arr(v)[0] == 2 && auto_arr(v)[1] == 3 && auto_arr(v)[2] == 4);

	auto v_first = arrview_first(3, a);
	static_assert(ARRAY_SIZE(v_first) == 3);
	assert(auto_arr(v_first)[0] == 1 && auto_arr(v_first)[2] == 3);

	auto v_last = arrview_last(3, a);
	static_assert(ARRAY_SIZE(v_last) == 3);
	assert(auto_arr(v_last)[0] == 3 && auto_arr(v_last)[2] == 5);

	auto v_shrink = arrview_shrink(1, 1, a);
	static_assert(ARRAY_SIZE(v_shrink) == 3);
	assert(auto_arr(v_shrink)[0] == 2 && auto_arr(v_shrink)[2] == 4);

	auto v_front = arrview_cfront(2, a);
	static_assert(ARRAY_SIZE(v_front) == 3);
	assert(auto_arr(v_front)[0] == 3);

	auto v_back = arrview_cback(2, a);
	static_assert(ARRAY_SIZE(v_back) == 3);
	assert(auto_arr(v_back)[2] == 3);

	auto v_full = arrview_full(a);
	static_assert(ARRAY_SIZE(v_full) == ARRAY_SIZE(a));
	assert(v_full == &a);

	//auto form and declaring form must produce the same type and value
	make_arrview(m_view, 1, 3, a);
	static_assert(is_same_array_element_type(v, m_view) == true);
	assert(v == m_view);

	//constness is part of the type and must be carried over
	const int c[] = {1,2,3,4,5};
	auto v_const = arrview_first(2, c);
	static_assert(ARRAY_SIZE(v_const) == 2);
	static_assert(is_pointer_to_const(*v_const) == true);
	static_assert(is_pointer_to_const(*v_first) == false);

	int m[2][3] = {{1,2,3},{4,5,6}};
	auto v_flat = arrview_flat(m);
	static_assert(ARRAY_SIZE(v_flat) == 6);
	assert(auto_arr(v_flat)[5] == 6);

	int f[6] = {1,2,3,4,5,6};
	auto v_dim = arrview_dim(3, f);
	static_assert(ARRAY_SIZE(v_dim) == 2);
	static_assert(ARRAY_SIZE(auto_arr(v_dim)[0]) == 3);
	assert(auto_arr(v_dim)[1][2] == 6);

	//string view drops the trailing NUL
	auto v_str = arrview_str("poor");
	static_assert(ARRAY_SIZE(v_str) == 4);
	assert(auto_arr(v_str)[0] == 'p' && auto_arr(v_str)[3] == 'r');

	uint32_t u = 0;
	auto v_bytes = arrview_bytes(&u);
	static_assert(ARRAY_SIZE(v_bytes) == sizeof(uint32_t));

	//runtime sized array and runtime sized view
	size_t len = 5;
	int vla[len];
	copy_array(vla, (int[]){1,2,3,4,5});
	auto v_vla = arrview_first(len - 2, vla);
	assert(ARRAY_SIZE(v_vla) == 3);
	assert(auto_arr(v_vla)[0] == 1 && auto_arr(v_vla)[2] == 3);

	//library macros keep expanding inside POOR_PRIMITIVE_CAT
	auto v_cat = POOR_PRIMITIVE_CAT(arr, view(1, 3, a));
	static_assert(is_same_array_element_type(v_cat, v) == true);
	assert(v_cat == v);

	//views are writable through auto and alias the original storage
	auto v_write = arrview_last(2, a);
	auto_arr(v_write)[0] = 40;
	assert(a[3] == 40);

	return 0;
}

static int array_insert_test(void) {
	{
		int x[] = {0,1,2,3,4,5};
		make_arrview_shrink(xv,1,1,x);
		array_insert(xv, 0, 99);
		assert(auto_arr(xv)[0] == 99);
		assert(*array_last_ref(xv) == 3);

		assert(*array_first_ref(x) == 0);
		assert(*array_last_ref(x) == 5);
		assert(auto_arr(x)[1] == 99);
		assert(auto_arr(x)[2] == 1);
	}
	{
		int x[] = {0,1,2,3,4,5};
		make_arrview_shrink(xv,1,1,x);
		array_insert(xv, 3, 99);
		assert(*array_last_ref(xv) == 99);

		assert(*array_first_ref(x) == 0);
		assert(*array_last_ref(x) == 5);

		assert(auto_arr(x)[4] == 99);
		assert(auto_arr(x)[3] == 3);
	}

	return 0;
}

static int sprint_array_test(void) {
	char buf[5];
	assert(sprint_array(buf, 1, 2, 3, 4, 5) == 5);
	assert(strcmp(buf, "1234") == 0);

	char (*p)[8] = &(char[8]){0};
	assert(sprintln_array(p, "ab", 1) == 4);
	assert(strcmp(*p, "ab1\n") == 0);

	return 0;
}

static int print_array_test(void) {
	int empty[0];
	print_array(empty);
	print_array((int (*)[0])NULL);
	print_array_hex(empty);
	print_array((int[]){7});
	print_array((int[]){1, 2, 3});
	print_array_hex((unsigned char[]){0xab, 1});
#define h_print_row(r) print_array(r)
	print_array_fmt(h_print_row, (int[2][2]){{1, 2}, {3, 4}});
	size_t cols = 2;
	int vla2d[2][cols];
	vla2d[0][0] = 5; vla2d[0][1] = 6; vla2d[1][0] = 7; vla2d[1][1] = 8;
	print_array_fmt(h_print_row, vla2d);
	println("done");
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(auto_arr_test),
	TEST_FN(array_size),
	TEST_FN(arrays_size),
	TEST_FN(fill_array_test),
	TEST_FN(foreach_array_ref_test),
	TEST_FN(foreach_array_index_test),
	TEST_FN(foreach_array_index_bw_test),
	TEST_FN(foreach_array_ref_zero_length_test),
	TEST_FN(foreach_array_ref_null_zero_length_test),
	TEST_FN(foreach_array_const_ref_zero_length_test),
	TEST_FN(foreach_array_const_ref_null_zero_length_test),
	TEST_FN(foreach_array_ref_bw_zero_length_test),
	TEST_FN(foreach_array_ref_bw_null_zero_length_test),
	TEST_FN(foreach_array_const_ref_bw_zero_length_test),
	TEST_FN(foreach_array_const_ref_bw_null_zero_length_test),
	TEST_FN(foreach_array_index_zero_length_test),
	TEST_FN(foreach_array_index_null_zero_length_test),
	TEST_FN(foreach_array_index_bw_zero_length_test),
	TEST_FN(foreach_array_index_bw_null_zero_length_test),
	TEST_FN(array_accessors),
	TEST_FN(copy_array_single),
	TEST_FN(copy_array_multiple),
	TEST_FN(same_type_arrays),

	TEST_FN(merged_array_test),
	TEST_FN(arrview_simple),
	TEST_FN(arrview_first_test),
	TEST_FN(arrview_last_test),
	TEST_FN(arrview_shrink_test),

	TEST_FN(array_dim_flat_test),

	TEST_FN(string_literal_test),
	TEST_FN(array_ptr_test),
	TEST_FN(arrview_auto_test),

	TEST_FN(array_insert_test),
	TEST_FN(sprint_array_test),
	TEST_FN(print_array_test),
};

static void usage(void) {
	fprintf(stderr, "usage: this_program [test_name]\n\n"
		   "available tests:\n");

	for(struct tests_struct *cur = &tests[0]; cur != &tests[sizeof(tests) / sizeof(tests[0])]; cur++ ){
		fprintf(stderr, "\t%s\n", cur->test_name);
	}
}

int main(int argc, char **argv) {
	if(argc != 2)
		return usage(), 1;

	for(struct tests_struct *cur = &tests[0]; cur != &tests[sizeof(tests) / sizeof(tests[0])]; cur++ ){
		if(!strcmp(argv[1], cur->test_name)) {
			return cur->fn();
		}
	}

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
