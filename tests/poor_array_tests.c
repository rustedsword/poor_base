#include <poor_array.h>
#include <stdio.h>
#include <assert.h>

#undef NDEBUG

#if defined __STDC_VERSION__ && __STDC_VERSION__ <= 201710L

#undef static_assert
#define static_assert_m(_expr_, _string_) _Static_assert(_expr_, _string_)
#define static_assert_s(_expr_) _Static_assert(_expr_, "static_assert")
#define static_assert(...) IF_SINGLE_ARG(static_assert_s ,static_assert_m ,__VA_ARGS__)(__VA_ARGS__)

#else

#undef static_assert
#define static_assert(_expr_, ...) assert(_expr_)

#endif //__STDC_VERSION__

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

	return 0;
}

static int same_type_arrays(void) {
	int i1[1];
	const int i1c[1];
	int (*i1p)[1] = &i1;
	const int (*i1cp)[1] = &i1c;
	int (*const ci1p)[1] = &i1;
	const int (*const ci1cp)[1] = &i1c;

	int i2[2];
	const int i2c[2];
	int (*i2p)[2] = &i2;
	const int (*i2cp)[2] = &i2c;
	int (*const ci2p)[2] = &i2;
	const int (*const ci2cp)[2] = &i2c;

	short s1[1];
	const short s1c[1];
	short (*s1p)[1] = &s1;
	const short (*s1cp)[1] = &s1c;
	short (*const cs1p)[1] = &s1;
	const short (*const cs1cp)[1] = &s1c;

	static_assert(is_arrays_of_same_types(i1, i1c) == true);
	static_assert(is_arrays_of_same_types(i1, i1p) == true);
	static_assert(is_arrays_of_same_types(i1, i1cp) == true);
	static_assert(is_arrays_of_same_types(i1, ci1p) == true);
	static_assert(is_arrays_of_same_types(i1, ci1cp) == true);

	static_assert(is_arrays_of_same_types(i1c, i1c) == true);
	static_assert(is_arrays_of_same_types(i1c, i1p) == true);
	static_assert(is_arrays_of_same_types(i1c, i1cp) == true);
	static_assert(is_arrays_of_same_types(i1c, ci1p) == true);
	static_assert(is_arrays_of_same_types(i1c, ci1cp) == true);

	static_assert(is_arrays_of_same_types(i1p, i1c) == true);
	static_assert(is_arrays_of_same_types(i1p, i1p) == true);
	static_assert(is_arrays_of_same_types(i1p, i1cp) == true);
	static_assert(is_arrays_of_same_types(i1p, ci1p) == true);
	static_assert(is_arrays_of_same_types(i1p, ci1cp) == true);

	static_assert(is_arrays_of_same_types(i1cp, i1c) == true);
	static_assert(is_arrays_of_same_types(i1cp, i1p) == true);
	static_assert(is_arrays_of_same_types(i1cp, i1cp) == true);
	static_assert(is_arrays_of_same_types(i1cp, ci1p) == true);
	static_assert(is_arrays_of_same_types(i1cp, ci1cp) == true);

	static_assert(is_arrays_of_same_types(ci1p, i1c) == true);
	static_assert(is_arrays_of_same_types(ci1p, i1p) == true);
	static_assert(is_arrays_of_same_types(ci1p, i1cp) == true);
	static_assert(is_arrays_of_same_types(ci1p, ci1p) == true);
	static_assert(is_arrays_of_same_types(ci1p, ci1cp) == true);

	static_assert(is_arrays_of_same_types(ci1cp, i1c) == true);
	static_assert(is_arrays_of_same_types(ci1cp, i1p) == true);
	static_assert(is_arrays_of_same_types(ci1cp, i1cp) == true);
	static_assert(is_arrays_of_same_types(ci1cp, ci1p) == true);
	static_assert(is_arrays_of_same_types(ci1cp, ci1cp) == true);

	/* int[1] vs int[2] */
	static_assert(is_arrays_of_same_types(i2, i1c) == true);
	static_assert(is_arrays_of_same_types(i2, i1p) == true);
	static_assert(is_arrays_of_same_types(i2, i1cp) == true);
	static_assert(is_arrays_of_same_types(i2, ci1p) == true);
	static_assert(is_arrays_of_same_types(i2, ci1cp) == true);

	static_assert(is_arrays_of_same_types(i2c, i1c) == true);
	static_assert(is_arrays_of_same_types(i2c, i1p) == true);
	static_assert(is_arrays_of_same_types(i2c, i1cp) == true);
	static_assert(is_arrays_of_same_types(i2c, ci1p) == true);
	static_assert(is_arrays_of_same_types(i2c, ci1cp) == true);

	static_assert(is_arrays_of_same_types(i2p, i1c) == true);
	static_assert(is_arrays_of_same_types(i2p, i1p) == true);
	static_assert(is_arrays_of_same_types(i2p, i1cp) == true);
	static_assert(is_arrays_of_same_types(i2p, ci1p) == true);
	static_assert(is_arrays_of_same_types(i2p, ci1cp) == true);

	static_assert(is_arrays_of_same_types(i2cp, i1c) == true);
	static_assert(is_arrays_of_same_types(i2cp, i1p) == true);
	static_assert(is_arrays_of_same_types(i2cp, i1cp) == true);
	static_assert(is_arrays_of_same_types(i2cp, ci1p) == true);
	static_assert(is_arrays_of_same_types(i2cp, ci1cp) == true);

	static_assert(is_arrays_of_same_types(ci2p, i1c) == true);
	static_assert(is_arrays_of_same_types(ci2p, i1p) == true);
	static_assert(is_arrays_of_same_types(ci2p, i1cp) == true);
	static_assert(is_arrays_of_same_types(ci2p, ci1p) == true);
	static_assert(is_arrays_of_same_types(ci2p, ci1cp) == true);

	static_assert(is_arrays_of_same_types(ci2cp, i1c) == true);
	static_assert(is_arrays_of_same_types(ci2cp, i1p) == true);
	static_assert(is_arrays_of_same_types(ci2cp, i1cp) == true);
	static_assert(is_arrays_of_same_types(ci2cp, ci1p) == true);
	static_assert(is_arrays_of_same_types(ci2cp, ci1cp) == true);

	/* int vs short */
	static_assert(is_arrays_of_same_types(s1, i1c) == false);
	static_assert(is_arrays_of_same_types(s1, i1p) == false);
	static_assert(is_arrays_of_same_types(s1, i1cp) == false);
	static_assert(is_arrays_of_same_types(s1, ci1p) == false);
	static_assert(is_arrays_of_same_types(s1, ci1cp) == false);

	static_assert(is_arrays_of_same_types(s1c, i1c) == false);
	static_assert(is_arrays_of_same_types(s1c, i1p) == false);
	static_assert(is_arrays_of_same_types(s1c, i1cp) == false);
	static_assert(is_arrays_of_same_types(s1c, ci1p) == false);
	static_assert(is_arrays_of_same_types(s1c, ci1cp) == false);

	static_assert(is_arrays_of_same_types(s1p, i1c) == false);
	static_assert(is_arrays_of_same_types(s1p, i1p) == false);
	static_assert(is_arrays_of_same_types(s1p, i1cp) == false);
	static_assert(is_arrays_of_same_types(s1p, ci1p) == false);
	static_assert(is_arrays_of_same_types(s1p, ci1cp) == false);

	static_assert(is_arrays_of_same_types(s1cp, i1c) == false);
	static_assert(is_arrays_of_same_types(s1cp, i1p) == false);
	static_assert(is_arrays_of_same_types(s1cp, i1cp) == false);
	static_assert(is_arrays_of_same_types(s1cp, ci1p) == false);
	static_assert(is_arrays_of_same_types(s1cp, ci1cp) == false);

	static_assert(is_arrays_of_same_types(cs1p, i1c) == false);
	static_assert(is_arrays_of_same_types(cs1p, i1p) == false);
	static_assert(is_arrays_of_same_types(cs1p, i1cp) == false);
	static_assert(is_arrays_of_same_types(cs1p, ci1p) == false);
	static_assert(is_arrays_of_same_types(cs1p, ci1cp) == false);

	static_assert(is_arrays_of_same_types(cs1cp, i1c) == false);
	static_assert(is_arrays_of_same_types(cs1cp, i1p) == false);
	static_assert(is_arrays_of_same_types(cs1cp, i1cp) == false);
	static_assert(is_arrays_of_same_types(cs1cp, ci1p) == false);
	static_assert(is_arrays_of_same_types(cs1cp, ci1cp) == false);

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

	TEST_FN(array_insert_test),
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
