// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2020 Alexandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_ARRAY_H
#define POOR_ARRAY_H

#include <poor_map.h>
#include <poor_stdio.h>
#include <poor_traits.h>
#include <string.h>

/* Some colors */
#define CRED    "\033[0;31m"
#define CGREEN  "\033[0;32m"
#define CYELLOW "\033[0;33m"
#define CBLUE   "\033[0;34m"
#define CMAGNETA "\033[0;35m"
#define CCYAN   "\033[0;36m"
#define CRESET  "\033[0m"

/* Define ARRAY_RUNTIME_CHECKS to one of three values to activate various checks when using macros from this library.
 *
 * NO_CHECK - no checks will be performed at compile-time or runtime when creating views,
 *	copying arrays and other macros. But your compiler may still catch some errors if any.
 *
 * STATIC_CHECK - compile-time checks will be generated where possible, this will allow
 *	to catch generation of out-of-bounds views or array copies where parameters are known at compile time.
 *	This option will increase compilation time.
 *
 * RUNTIME_CHECK - same as STATIC_CHECK, but also with run-time checks where non constant values are used.
 *	i.e creating an array view from VLA, or creating a view from static array but with variable size.
 *	This option will incur run-time overhead, so use this only for debugging.
*/
#define NO_CHECK		0
#define STATIC_CHECK		1
#define RUNTIME_CHECK		2
#define POOR_ARRAY_CHECK	STATIC_CHECK

/* arr_errmsg() will be called when POOR_ARRAY_CHECK is set to RUNTIME_CHECK when error occurs */
#ifndef arr_errmsg
#define arr_errmsg(...) ((void)printerrln(__VA_ARGS__), (void)abort(), 0)
#endif

/***** Main API ******/

/* auto_arr(_arrm_)
 * @_arrm_: an array or a pointer to an array
 *
 * This macro returns (_arrm_) if (_arrm_) is an array, or (*_arrm_) if (_arrm_) is a pointer to an array
 * Basically, this macro automatically dereferences pointers to arrays.
 * Stops compilation if (_arrm_) is not an array or a pointer to array.
 *
 * Provides base for checking arrays and hides complexity when
 * working with fixed or variable arrays and pointers to them.
 *
 * for direct usage it is reccomended to use arr() macro alias to type less.
 *
 * example:

	char str[] = "String";
	char (*str_ptr) [ARRAY_SIZE(str)] = &str;

	//These two operations are the same:
	arr(str)[1] = 'p';
	arr(str_ptr)[1] = 'r';

	//Works with multi-dimensional arrays too:
	char   array [2][5][4] = {0};
	char (*arptr)[2][5][4] = &array;

	arr(array)[0][0][1] = 'R';
	arr(arptr)[0][1][0] = 'F';

	println( arr(arptr)[0][0][1] ); //Prints 'R'
	println( arr(array)[0][1][0] ); //Prints 'F'
*/
#define auto_arr(...) h_auto_arr((__VA_ARGS__))

/* Macro alias for auto_arr() */
#if !defined(arr)
#define arr(...) auto_arr(__VA_ARGS__)
#endif

/*** Array informational macros ***/

/* Returns number of elements in the array */
#define ARRAY_SIZE(...) UNSAFE_ARRAY_SIZE(auto_arr(__VA_ARGS__))
/* Returns total array size in bytes */
#define ARRAY_SIZE_BYTES(...) UNSAFE_ARRAY_SIZE_BYTES(auto_arr(__VA_ARGS__))
/* Returns size in bytes of the one element in the array */
#define ARRAY_ELEMENT_SIZE(...) UNSAFE_ARRAY_ELEMENT_SIZE(auto_arr(__VA_ARGS__))
/* Extracts type of array element from array */
#define ARRAY_ELEMENT_TYPE(...) UNSAFE_ARRAY_ELEMENT_TYPE(auto_arr(__VA_ARGS__))

/* ARRAYS_SIZE(_arrm_1_, ..., _arrm_n_)
 * Get sum of array sizes
 * Arguments should be arrays or pointers to arrays
 * example:

	char te[] = {1, 2, 3};
	long (*me)[2] = &(long[]){5, 6};
	println(ARRAYS_SIZE(te, me)); //prints: 5
 */
#define ARRAYS_SIZE(...) ( MAP_SEP((+), ARRAY_SIZE, __VA_ARGS__) )

/* ARRAYS_SIZE_BYTES(_arrm_1_, ..., _arrm_n_)
 * Get total size in bytes for all arrays
 * Arguments should be arrays or pointers to arrays
 * example:

	uint8_t te[] = {1, 2, 3};
	long (*me)[2] = &(long[]){5, 6};
	println(ARRAYS_SIZE_BYTES(te, me)); //prints: 19  (if sizeof(long) == 8, then (8 * 2) + 3 )
 */
#define ARRAYS_SIZE_BYTES(...) ( MAP_SEP((+), ARRAY_SIZE_BYTES, __VA_ARGS__) )

/* PRINT_ARRAY_INFO(_arrm_): Prints information about array
 * @_arrm_: an array or a pointer to an array
 * example:

	PRINT_ARRAY_INFO(&(long long[]){1, 2, 3, 4});
	//prints: Array "&(long long[]){1, 2, 3, 4}" at 0x7fffffffdc40 has size:4 uses 32 bytes, while single element uses 8 bytes

	...
	int main(int argc, char **argv) {
		short (*test)[argc * 3];
		malloc_array(test);

		PRINT_ARRAY_INFO(test);
		//if argc == 1, prints: VLA "test" at 0x5555555592a0 has size:3 uses 6 bytes, while single element uses 2 bytes

		free(test);
		return 0;
	}
 */
#define PRINT_ARRAY_INFO(...)									\
	println(h_print_array_info((__VA_ARGS__), " \"" #__VA_ARGS__ "\" at "),			\
		((const void*)(__VA_ARGS__)),							\
		" has size:", ARRAY_SIZE((__VA_ARGS__)),					\
		" uses ", ARRAY_SIZE_BYTES((__VA_ARGS__)), " bytes,"				\
		" while single element uses ", ARRAY_ELEMENT_SIZE((__VA_ARGS__)), " bytes")

/* print_array(_arrm_)
 * Prints array as formatted output [x,y,z,...]
 * @_arrm_: an array or a pointer to an array
 * Type of array elements should be one of standard C types. See println() for more info.
 * example:

	print_array((int[]){1,2,3,4}); //prints: [1,2,3,4];

	const char * strings[4] = {
		"First",
		"Second",
		"Third",
		"Fourth",
	};
	print_array(strings); //prints: [First,Second,Third,Fourth]
 */
#define print_array(...) do {						\
	const make_arrview_full(_tmp_arr_ptr_, __VA_ARGS__);		\
	unsafe_make_array_first_ref(_tmp_arr_ptr_, _ref_);		\
										\
	print((char)'[', *_ref_);						\
	for(_ref_++; _ref_ != unsafe_array_end_ref(_tmp_arr_ptr_); _ref_++)	\
		print((char)',', *_ref_);					\
										\
	println("]");								\
} while (0)

/*** Basic array manipulation ***/

/* make_array_ptr(name, pointer, size)
 *
 * Declares a pointer to an array with specified name and sets it's value to (pointer)
 * This macro is useful to tie together pointer and size in the form of pointer to array.
 *
 * @pointer: pointer to the first array element
 * @size: size of the array, should be greater than zero.
 *
 * example:

	//Wraping pointer and size returned from function
	size_t size;
	int *ptr = function_which_returns_pointer_and_size(&size);
	if(!ptr || !size)
		return;

	make_array_ptr(data, ptr, size);
	print_array(data);

	...
	//Simple program:

	int main(int argc, char **argv) {
		make_array_ptr(args, argv, argc);
		print_array(args);
		return 0;
	}

 */
#define make_array_ptr(_name_, _pointer_, _size_) typeof(*(_pointer_)) (* _name_)[(_size_)]  = (void*)(_pointer_)

/* malloc_array(_arrp_)
 * Allocates memory for pointer to an array.
 * @_arrp_: a pointer to an array
 * Provide pointer to an array, to allocate memory. Pointer can be pointer to VLA.
 * Size of allocation will be deduced from pointer type;
 *
 * returns newly allocated pointer if allocation successful, or NULL on failure;
 *
 * usage example:

	int (*some_ints)[ rand() % 10 + 1 ]; //Create pointer to VLA with random size from 1 to 10

	if(!malloc_array(some_ints)) {
		printerrln("Failed to alloc array");
		exit(EXIT_FAILURE);
	}

	for(unsigned i = 0; i < P_ARRAY_SIZE(some_ints); i++) {
		(*some_ints)[i] = ((int)i + 1) * 2;
	}
	println("index 0 has value:", (*some_ints)[0]);

	free(some_ints);

*/
#define malloc_array(_arrp_) (( _arrp_ = malloc( ARRAY_SIZE_BYTES(_arrp_) ) ))

/* Same as malloc_array, but with calloc */
#define calloc_array(_arrp_) (( _arrp_ = calloc( 1, ARRAY_SIZE_BYTES(_arrp_) ) ))

/* memset_array(_arrm_, _symbol_)
 * Fills array with specified byte by using memset
 * @_arrm_: an array or a pointer to an array
 * @_symbol_: a byte for array filling
 * WARNING: It is better and safer to use fill_array() instead!
 * example:

	char (*data)[5];
	malloc_array(data);
	memset_array(data, 'L');

	foreach_array_ref(data, ref)
		print(*ref); //Will print: LLLLL

*/
#define memset_array(_arrm_, _symbol_) memset((_arrm_), _symbol_, ARRAY_SIZE_BYTES(_arrm_))

/* fill_array(_arrm_, _value_)
 * Fills array with specified value
 * @_arrm_: an array or a pointer to an array
 * example:

	int (*ty)[5];
	malloc_array(ty);

	fill_array(ty, 50);
	print_array(ty); //Will print [50,50,50,50,50]

	free(ty);
*/
#define fill_array(_arrm_, ...)  \
	foreach_array_ref(_arrm_, _ref_) \
		*(_ref_) = (__VA_ARGS__)

/*** Array iterators ***/

/* Iterate over an array.
 * @_arr_ptr_: array or pta
 * @_ref_ptr_name_: name of pointer which will be used as reference to each array element
 * example:

	uint16_t test[] = {1, 2, 3, 4};
	foreach_array_ref(&test, val)
		print(*val);  //prints: 1234

 *
 * NOTE: if _arr_ptr_ contains const data, then _ref_ptr_name_ also will be const
 */
#define foreach_array_ref(_arr_, _ref_ptr_name_) foreach_array_ref_base(,(_arr_), _ref_ptr_name_)

/* same as foreach_array_ref(), but _ref_ptr_name_ is always const */
#define foreach_array_const_ref(_arr_, _ref_ptr_name_) foreach_array_ref_base(const, (_arr_), _ref_ptr_name_)

/*
 * Iterate backwards over an array. Same as foreach_array_ref(), but backwards.
 * example:
 *
	uint16_t test[] = {1, 2, 3, 4};
	foreach_array_ref_bw(&test, val)
		print(*val);  //prints: 4321
 */
#define foreach_array_ref_bw(_arr_, _ref_ptr_name_) foreach_array_ref_bw_base(,(_arr_), _ref_ptr_name_)

/* same as foreach_array_ref_bw(), but _ref_ptr_name_ is always const */
#define foreach_array_const_ref_bw(_arr_, _ref_ptr_name_) foreach_array_ref_bw_base(const, (_arr_), _ref_ptr_name_)

/*** Array accessors ***/

/* get pointer to the first, to the last and to the one past the last element of the array  */
#define array_first_ref(_array_ptr_) (& auto_arr((_array_ptr_))[0])
#define array_last_ref(_array_ptr_)  (& auto_arr((_array_ptr_))[ARRAY_SIZE(_array_ptr_) - 1])
#define array_end_ref(_array_ptr_)   (& auto_arr((_array_ptr_))[ARRAY_SIZE(_array_ptr_)])

/* returns true if ref points to the first, to the last or the one past the last elemenet of ther array */
#define is_first_array_ref(_arr_ptr_, _ref_) ((_ref_) == array_first_ref((_arr_ptr_)))
#define is_last_array_ref(_arr_ptr_, _ref_)  ((_ref_) == array_last_ref((_arr_ptr_)))
#define is_end_array_ref(_arr_ptr_, _ref_)   ((_ref_) == array_end_ref((_arr_ptr_)))
#define is_first_or_last_array_ref(_arr_ptr_, _ref_) ( is_first_array_ref((_arr_ptr_), (_ref_)) || is_last_array_ref((_arr_ptr_), (_ref_))  )

/* Get index of an array element where _ref_ is pointing.
 * @_arrm_: array or pointer to array
 * @_ref_: pointer to an element inside that array
 * example:

	int a[] = {10, 20, 30};
	foreach_array_ref(a, ref)
		println("array index:", array_ref_index(a, ref), " value:", *ref);

	//prints:
	//array index:0 value:10
	//array index:1 value:20
	//array index:2 value:30

*/
#define array_ref_index(_arrm_, _ref_) ((_ref_) - array_first_ref(_arrm_))

/*** Array Copy ***/

/* copy_array(_arrm_dst_, _arrm_src_)
 * @_arrm_dst_: destination array or a pointer to an array
 * @_arrm_src_: source array or a pointer to an array
 * Copies data from src array or pta to dst array or pta per element
 * if dst array is larger or equal than source array, then entire source array will be copied to dst.
 * excess bytes in dst won't be touched.
 *
 * if dst array is smaller than src array, then only part of source will be copied from src with size equals dst array.
 *
 * dst and src arrays can be of different type. i.e. it is valid to copy data from array of ints to array of longs.
 * But compiler may complain if losing precision.
 *
 * @dst_ptr: destination array or pta
 * @__VA_ARGS__: source array or pta
 *
 * examples:

	//Destination is larger than source:

	int dst[5] = {1,2,3,4,5};
	int src[3] = {9,8,7};
	copy_array(dst, src);
	print_array(dst); //prints: [9,8,7,4,5]
	...
	//Source and Destination have different, but compatible types
	//Source is larger than Destination

	long dst[]  = {1,2};
	short src[] = {9,8,7};
	copy_array(dst, src);
	print_array(dst); //prints: [9,8]
	...
	//Dynamic array initialization:

	char (*string)[40];
	malloc_array(string);
	copy_array(string, "Hello!");
	println(*string);
	free(string);
	...
	//VLA Initialization and array of structs

	struct toto {
		int val;
		char *string;
	};

	size_t len = 10;
	struct toto values[len];
	fill_array(values, (const struct toto){.val = -1, .string = "None"});
	copy_array(values, (const struct toto[]){
				   {.val = 1,  .string = "First"},
				   {.val = 10, .string = "Second"},
				   {.val = 30, .string = "Third"},
				   });

	foreach_array_const_ref(values, ref)
		println(ref->val, " ", ref->string);

 */
#define copy_array(_arrp_dst_, ...) do {			\
	make_arrview_full(_tmp_dst_full_, _arrp_dst_);		\
	const make_arrview_full(_tmp_src_full_, (__VA_ARGS__));	\
	unsafe_copy_array(_tmp_dst_full_, _tmp_src_full_);	\
} while(0)

/* copy_arrays(_arrm_dst_, _arrm_src_1_, ..., _arrm_src_n_)
 * Copies multiple arrays into a single array
 * @_arrm_dst_: destination array or pointer to destination array
 * @_arrm_src_: source arrays and/or pointers to source arrays to copy data from
 *
 * All source arrays must contain same element type as destination array
 * Destination array size must be equal or greater than total size of all source arrays.
 * Source arrays should not overlap with destination array, but may overlap with each other.
 *
 * example:

	//String concatenation
	make_arrview_str(src1, "This ");
	make_arrview_str(src2, "Is ");
	make_arrview_str(src3, "a string");

	char dst[ARRAYS_SIZE(src1, src2, src3)];
	copy_arrays(dst, src1, src2, src3);

	print_array(dst); //prints: [T,h,i,s, ,I,s, ,a, ,s,t,r,i,n,g]

	//copy integers from various arrays
	int s1[2] = {1,2};
	int (*s2)[4] = &(int[4]){3,4,5,6};
	int s3[(size_t){3}]; s3[0] = 7; s3[1] = 8; s3[2] = 9;

	int (*dst)[ARRAYS_SIZE(s1, s2, s3)] = malloc_array(dst);
	if(dst) {
		copy_arrays(dst, s1, s2, s3);
		print_array(dst); //prints: [1,2,3,4,5,6,7,8,9]

		free(dst);
	}
*/
#define copy_arrays(_arrm_dst_, ...) (							\
	(void)h_copy_arrs_chk_size_sel(_arrm_dst_, __VA_ARGS__),			\
	(void)RECURSION_ARG(h_copy_arrs, _arrm_dst_, _arrm_dst_, __VA_ARGS__)		\
)

/*** Arrview ***/

/* make_arrview(name, start, size, src):
 * make an array view of the array src with size (size) starting from (start) position.
 * example:

	uint8_t data[] = {0,1,2,3,4,5};
	make_arrview(data_slc, 2, 3, &data); //Start index: 2 and size: 3
	print_array(data_slc); //prints: [2,3,4]
 */
#define make_arrview(_name_, _idx_, _size_, ...) make_unsafe_arrview(_name_, (_idx_), (_size_), &auto_arr(__VA_ARGS__))
#define arrview(_idx_, _size_, ...) unsafe_arrview((_idx_), (_size_), &auto_arr(__VA_ARGS__))

/* make_arrview_first(name, size, src):
 * create arrview of the first (size) elements of the array src
 * example:

	uint8_t data[] = {0,1,2,3,4,5};
	make_arrview_first(data_slc, 2, &data);
	print_array(data_slc); //prints: [0,1]
 */
#define make_arrview_first(_name_, _size_, ...) unsafe_make_arrview_first(_name_, (_size_), &auto_arr(__VA_ARGS__))
#define arrview_first(_size_, ...) unsafe_arrview_first((_size_), &auto_arr(__VA_ARGS__))

/* make_arrview_last(name, size, src):
 * create arrview of the last (size) elements of the array src
 * example:

	uint8_t data[] = {0,1,2,3,4,5};
	make_arrview_last(data_slc, 2, &data);
	print_array(data_slc); //prints: [4,5]
 */
#define make_arrview_last(_name_, _size_, ...) unsafe_make_arrview_last(_name_, (_size_), &auto_arr(__VA_ARGS__))
#define arrview_last(_size_, ...) unsafe_arrview_last((_size_), &auto_arr(__VA_ARGS__))

/* make_arrview_shrink(name, skip_start, skip_end, src):
 * Make an array view without first (skip_start) elements
 * and without last (skip_end) elements of the array (src)
 *
 * example:

	uint8_t data[] = {0,1,2,3,4,5};
	make_arrview_shrink(data_slc, 2, 1, &data);
	print_array(data_slc); //prints: [2,3,4]
*/
#define make_arrview_shrink(_name_, _skip_start_, _skip_end_, ...) \
	unsafe_make_arrview_shrink(_name_, (_skip_start_), (_skip_end_), &auto_arr(__VA_ARGS__))
#define arrview_shrink(_skip_start_, _skip_end_, ...) \
	unsafe_arrview_shrink((_skip_start_), (_skip_end_), &auto_arr(__VA_ARGS__))

/* make_arrview_cfront(name, skip_start, src):
 * make an arrview without first (skip_start) elements of the array src
 * example:

	uint8_t data[] = {0,1,2,3,4,5};
	make_arrview_cfront(data_slc, 3, &data);
	print_array(data_slc); //prints: [3,4,5]
*/
#define make_arrview_cfront(_name_, _skip_start_, ...) unsafe_make_arrview_cfront(_name_, (_skip_start_), &auto_arr(__VA_ARGS__))
#define arrview_cfront(_skip_start_, ...) unsafe_arrview_cfront((_skip_start_), &auto_arr(__VA_ARGS__))

/* make_arrview_cback(name, skip_end, src):
 * make an arrview without last (skip_end) elements of the array src
 * example:

	uint8_t data[] = {0,1,2,3,4,5};
	make_arrview_cback(data_slc, 3, &data);
	print_array(data_slc); //prints: [0,1,2]
*/
#define make_arrview_cback(_name_, _skip_end_, ...) unsafe_make_arrview_cback(_name_, _skip_end_, &auto_arr(__VA_ARGS__))
#define arrview_cback(_skip_end_, ...) unsafe_arrview_cback(_skip_end_, &auto_arr(__VA_ARGS__))

/* make_arrview_dim(_size_, _arrm_)
 * Creates an arrview for array by splitting it's top dimension into two dimensions
 * @_size_: size of second dimension, should not be less than source array size
 * @_arrm_: source array or a pointer to an array
 *
 * int a[40] -> size:4 -> int (*ptr)[10][4];
 * int a[7]  -> size:3 -> int (*ptr)[2][3]; //Last element won't be part of the view
 *
 * example:

	const char *k_v[] = {
		"1", "Flower",
		"2", "Blast",
		"3", "Light"
	};

	//Reinterpret k_v array as pairs
	make_arrview_dim(kv_pair_view, 2, k_v);
	foreach_array_ref(kv_pair_view, pair_ref)
		print_array(pair_ref);

	//prints:[1,Flower]
	//	 [2,Blast]
	//	 [3,Light]


	//Reinterpret k_v array as triples
	make_arrview_dim(kv_triple_view, 3, k_v);
	foreach_array_ref(kv_triple_view, triple_ref)
		print_array(triple_ref);

	//prints:[1,Flower,2]
	//	 [Blast,3,Light]

 */
#define make_arrview_dim(_name_, _size_, ...) h_make_arrview_dim(_name_, (_size_), (__VA_ARGS__))
#define arrview_dim(_size_, ...) h_arrview_dim((_size_), (__VA_ARGS__))

/* make_arrview_flat(_arrm_)
 * Creates an arrview for multi-dimensional array by merging it's first two dimensions into a single dimension
 * @_arrm_: source array or a pointer to an array. Should contain at least two dimensions
 *
 * int a[4][3]    -> int (*ptr)[12]
 * int a[100][10] -> int (*ptr)[1000]
 *
 * example:

	int matrix[3][3] = {
		{1,2,3},
		{4,5,6},
		{7,8,9},
	};

	make_arrview_flat(matrix_flat, matrix);
	print_array(matrix_flat);
	//prints: [1,2,3,4,5,6,7,8,9]
 */
#define make_arrview_flat(_name_, ...) h_make_arrview_flat(_name_, (__VA_ARGS__))
#define arrview_flat(...) h_arrview_flat((__VA_ARGS__))

/* make_arrview_full(name, src):
 * make a full array view of the array src
 * example:

	uint8_t data[] = {0,1,2,3,4,5};
	make_arrview_full(data_slc, &data);
	print_array(data_slc); //prints: [0,1,2,3,4,5]
*/
#define make_arrview_full(_name_, ...) ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ARRAY_SIZE((__VA_ARGS__))] = arrview_full((__VA_ARGS__))
#define arrview_full(...) &(auto_arr((__VA_ARGS__)))

/* make_arrview_str(name, src):
 * make arrview without last element, useful for cutting '\0' from C strings */
#define make_arrview_str(_name_, ...) make_arrview_cback(_name_, 1, __VA_ARGS__)
#define arrview_str(...) arrview_cback(1, __VA_ARGS__)

/* string_literal(_name_, string_literal)
 * @_name_: variable name for new array pointer
 * @string_literal: string literal, e.g "Some String"
 *
 * Special macro for string literals.
 * Declares constant pointer to array of constant chars and sets it's value to provided string literal
 * array size includes '\0'
 *
 * example:

	string_literal(str, "String Literal");
	//str = NULL; // <--Error
	//(*str)[0] = '1'; // <--Error

	//Dereference to const char* and print
	println(*str);
	printf("%s\n", *str);
	fwrite(*str, 1, ARRAY_SIZE(str) - 1, stdout);

	//Make slice without '\0' and print it as array of chars
	make_arrview_str(str_nonull, str);
	print_array(str_nonull);

* Also, you can declare string literals at global scope with static keyword (or without)

	static string_literal(useful_string, "I am just a string");

	int main() {
		println(*useful_string, " with size:", ARRAY_SIZE(useful_string));
		return 0;
	}
 */
#define string_literal(_name_, _string_) \
    declare_string_literal(_name_, _string_) = (const typeof( (_string_)[0] ) (*)[])( (_string_) )

/*** Experimental array macros. These macros need more testing ***/

/* make_merged_array(_name_, _arrm_src1_, ..., _arrm_srcn_)
 * @_name_: name of new array to create
 * @_arrm_src: source arrays and/or pointers to arrays to copy data from
 *
 * This macro creates an array with name _name_ with size
 * of all provided arrays and copies all these arrays' data into it.
 *
 * All provided arrays should be of same type(excluding qualifiers)
 *
 * If at least one of provided source arrays is VLA, then resulting array will be variable length too.
 *
 * Doesn't work with multi-dimensional arrays for now =\
 *
 * examples:

	make_merged_array(data,
			  ((int[]){0,1,2,3}),
			  ((int[]){4,5,6}),
			  ((int[]){7,8,9,10,11}));

	print_array(data); //prints: [0,1,2,3,4,5,6,7,8,9,10,11]

	...

	make_arrview_str(str_this,   "This");
	make_arrview_str(str_are,    "are");
	make_arrview_str(str_arrays, "ARRAYS");
	make_arrview_str(str_space,  " ");
	make_arrview_str(str_dot,    ".");
	make_arrview_str(str_null,   "\0");

	make_merged_array(data,
			  str_this, str_space,
			  str_are, str_space,
			  str_arrays, str_dot,
			  str_null);
	println(data);

 */
#define make_merged_array(_name_, ...) \
	ARRAY_ELEMENT_TYPE_NO_QUAL(TAKE_FIRST_ARG(__VA_ARGS__)) _name_ [ ARRAYS_SIZE(__VA_ARGS__) ]; \
	copy_arrays(_name_, __VA_ARGS__)

/* make_arrview_ref_ref(name, ref1, ref2):
 *
 * Create arrview from two pointers to some elements of the same array
 *
 * ref2 must be equal or greater than ref1
 *
 * created view will always be a pointer to VLA.
 *
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
 *           ^       ^
 * ref1------|       |
 * ref2--------------|
 *
 *           ^   ^   ^
 * view----| 2 | 3 | 4 |
 */
#define arrview_ref_ref(_ref1_, _ref2_ ) \
    (h_decl_arrview_ref_ref(,_ref1_, _ref2_))(_ref1_)

#define make_arrview_ref_ref(_name_, _ref1_, _ref2_) \
    h_decl_arrview_ref_ref(_name_, _ref1_, _ref2_) = (void*)(_ref1_)

#define h_decl_arrview_ref_ref(_name_, _ref1_, _ref2_) \
    typeof(*(_ref1_))(*_name_)[ (_ref2_) - (_ref1_) + 1]

/*** Experimental Vector-like array macros ***/

/* array_insert(arr, ref, val)
 * Moves backwards all data from the position where (ref) points
 * and writes value (val) at that position
 *
 * | 0 | 1 | 2 | 3 | 4 |
 *           ^
 * ref-------|
 *
 *         | 2 | 3 |
 *             \
 *              -->
 *                 \
 *             | 2 | 3 |
 *
 * | 0 | 1 | v | 2 | 3 |
 *           ^
 * val ------|
 *
 */
#define array_insert(_arr_, _ref_, _val_) (				\
	(void)memmove((_ref_) + 1,					\
		(_ref_),						\
		(array_last_ref(_arr_) - (_ref_)) * sizeof(*(_ref_))),	\
	(void)(*(_ref_) = (_val_))					\
	)

/* array_insert_array(arr, idx, src_arr)
 * Inserts data from array (src_arr) to array (arr) at specified position (idx)
 *
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
 *           ^
 * idx-------|
 * src_arr | 7 | 8 | 9 |
 *
 *         | 2 | 3 |
 *             \
 *              ---------->
 *                         \
 *                     | 2 | 3 |
 *
 * | 0 | 1 | 7 | 8 | 9 | 2 | 3 |
 *           ^   ^   ^
 * src_arr | 7 | 8 | 9 |
 *
 * example:
 *
    long v[] = {0,1,2,3,4,5,6};
    array_insert_array(v, 2, (long[]){7,8,9});
    print_array(v); //prints: [0,1,7,8,9,2,3]

 */
#define array_insert_array(_arr_, _idx_, ...) h_array_insert_array(_arr_, _idx_, (__VA_ARGS__))

/* array_remove_view(array, view)
 * Erases a range elements from the provided array
 * by moving forward all data in the array from
 * the position after the view end to the position where the view begins.
 *
 * (view) must be a pointer to an array which points to some range in the array
 * and it's range should not span behind the array.
 *
 * if view spans to the end of the array,
 * then this macro does nothing (since nothing to move forward)
 *
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 *           ^   ^   ^
 * view ---|   |   |   |
 *
 *                     | 5 | 6 | 7 |
 *                          /
 *                <---------
 *               /
 *         | 5 | 6 | 7 |
 *
 * | 0 | 1 | 5 | 6 | 7 | 5 | 6 | 7 |
 */
#define array_remove_view(_arr_ptr_, _view_)							\
	memmove(_view_, array_end_ref(_view_),							\
	(array_last_ref(_arr_ptr_) - array_last_ref(_view_)) * ARRAY_ELEMENT_SIZE(_arr_ptr_))	\

/* array_remove_view_fill(array, view, val)
 * works exactly same as array_remove_view()
 * while addititionaly fills last 'free' elements of the array with value (val)
 *
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 *           ^   ^   ^
 * view ---|   |   |   |
 *
 *                     | 5 | 6 | 7 | 8 |
 *                             /
 *                  <----------
 *                 /
 *         | 5 | 6 | 7 | 8 |
 *
 * | 0 | 1 | 5 | 6 | 7 | 8 | v | v | v |
 *                           ^   ^   ^
 * val -------------------------------
 *
 * example:
 *
    int v[] = {0,1,2,3,4,5,6,7,8};
    make_arrview(v_view, 2, 3, v); //pointer to elements '2', '3', '4'
    array_remove_view_fill(v, v_view, 9);
    print_array(v); //prints: [0,1,5,6,7,8,9,9,9]

*/
#define array_remove_view_fill(_arr_ptr_, _view_, _val_) do {		\
	array_remove_view(_arr_ptr_, _view_);				\
	make_arrview_last(_to_replace_, ARRAY_SIZE(_view_), _arr_ptr_);	\
	unsafe_fill_array(_to_replace_, _val_);				\
} while(0)

/* array_remove_ref(array, ref)
 * Erases a single element from the array by moving all data after (ref) towards front
 * of the array.
 *
 * ref should be a pointer to some element inside the array.
 *
 * if ref is a pointer to the last element of the array,
 * then this macro does nothing(since nothing to move forward)
 *
 * basically, this macro is the same as array_remove_view() with the view of size 1
 *
 * | 0 | 1 | 2 | 3 | 4 |
 *           ^
 * ref ------|
 *
 *             | 3 | 4 |
 *                 /
 *              <--
 *             /
 *         | 3 | 4 |
 *
 * | 0 | 1 | 3 | 4 | 4 |
 *
 * example:

	int v[] = {0,1,2,3,4};
	int *ptr = &v[2]; //pointer to value '2'
	array_remove_ref(v, ptr);
	print_array(v); //prints: [0,1,3,4,4]

*/
#define array_remove_ref(_arr_ptr_, _ref_)				\
	memmove((_ref_),						\
		(_ref_) + 1,						\
	(array_last_ref(_arr_ptr_) - (_ref_)) * sizeof(*(_ref_)))

/* array_remove_ref_fill(array, ref, val)
 * works same as array_remove_ref() while additionally sets
 * the last value of the array with value (val)
 *
 * | 0 | 1 | 2 | 3 | 4 |
 *           ^
 * ref ------|
 *
 *             | 3 | 4 |
 *                 /
 *              <--
 *             /
 *         | 3 | 4 |
 *
 * | 0 | 1 | 3 | 4 | v |
 *                   ^
 * val --------------|
 *
 * example:

	int v[] = {0,1,2,3,4};
	int *ptr = &v[2]; //pointer to value '2'
	array_remove_ref_fill(v, ptr, 9);
	print_array(v); //prints: [0,1,3,4,9]

 */
#define array_remove_ref_fill(_arr_ptr_, _ref_, _val_)	\
	((void) array_remove_ref(_arr_ptr_, _ref_),	\
	(void)(*array_last_ref(_arr_ptr_) = (_val_)))

/****** ------------------------------------------------------------------------------ *******/
/*** Implementation macros. Everything below should not be used and can be changed anytime ***/
/****** ------------------------------------------------------------------------------ *******/

/* expands one of three provided macros by checking current POOR_ARRAY_CHECK setting */
#define POOR_ARR_CHK_SEL(_no_check_, _static_check_, _runtime_check_) \
	TOKEN_CAT_2(CHECK_SELECT_, POOR_ARRAY_CHECK)(_no_check_, _static_check_, _runtime_check_)

#define CHECK_SELECT_0(_macro_, _1, _2)	_macro_
#define CHECK_SELECT_1(_0, _macro_, _2)	_macro_
#define CHECK_SELECT_2(_0, _1, _macro_)	_macro_
#define CHECK_SELECT_POOR_ARRAY_CHECK(_0, _macro_, _2)	_macro_

#define TAKE_FIRST_ARG(var, ...) var

/* Typeof with stripped qualifiers */
#define TYPEOF_NO_QUAL(var) typeof( (typeof(var) (*)(void) ){0}() )

/* Returns type of array element without qualifiers, works only on single-dimensional arrays */
#define ARRAY_ELEMENT_TYPE_NO_QUAL(var) TYPEOF_NO_QUAL(ARRAY_ELEMENT_TYPE(var))

/* Expands to string literal with file:line. example: /home/user/cool_program.c:56 */
#define FILE_AND_LINE __FILE__ ":" STRINGIFY2(__LINE__)

/* If _expr_ is not constant expression and is equals zero then this macro calls arr_errmsg() macro
 * If _expr_ is constant expression and is equals zero then terminates compilation  */
#define ARR_ASSERT_MSG(_expr_, ...) (void)(sizeof(char [_expr_ ? 1 : -1]) == 1 ? 0 : arr_errmsg(__VA_ARGS__))

/* If _expr_ is constant expression and is equals zero then terminates compilation  */
#define ARR_ASSERT(_expr_)  (void)(sizeof(char [_expr_ ? 1 : -1]))

/* Declares a pointer to array with same type as another pointer to array but with different size */
#define unsafe_make_arrptr(_name_, _size_, _arrp_) UNSAFE_ARRAY_ELEMENT_TYPE(*_arrp_)(* _name_)[_size_]

typedef union _dummy_type_ {char a;} _dummy_type_;
#define if_dummy_true(var, t, f) _Generic(&var, _dummy_type_(*)[1 + true]: t, default: f)
#define is_dummy_true(var) _Generic(&var, _dummy_type_(*)[1 + true]: true, default: false)

typedef struct _is_arr_ {int a;} _is_arr_;
typedef struct _is_p_arr_ {int a;} _is_p_arr_;

/**** auto_arr() implementation ****/
/* returns _is_arr_ if var is VLA
 * returns _is_p_arr if var is a pointer to VLA
 * or returns var as is */
#define h_arrvla_chk(var) if_vla(var, (_is_arr_){0}, if_vla(*var, (_is_p_arr_){0}, var))
/* returns _dummy_type_ if var is VLA */
#define vla_to_dummy(var) if_vla(var, (_dummy_type_){0}, var)

/* returns _is_arr_ if var is an array or VLA
 * returns _is_p_arr_ if var is a pointer to VLA
 * or returns dereferenced var if it is a pointer to any other type */
#define h_aa_l1(var)							\
	_Generic(& (typeof(h_arrvla_chk(var))){0},			\
		typeof(vla_to_dummy(*var))(*)[]: (_is_arr_*){0},	\
		_is_arr_ *: (_is_arr_*){0},				\
		_is_p_arr_ *: (_is_p_arr_*){0},				\
		default: *var)

/* var should not be vla or pointer to vla
 * returns 2 if var is array */
#define h_aa_l2(var)				\
	_Generic(&var,				\
		typeof(*var)(*)[]: 2,		\
		_is_p_arr_ **: 2,		\
		_is_arr_ **: 1			\
	)

#define h_auto_arr(var)					\
	_Generic(&(int [h_aa_l2(h_aa_l1(var))]){0},	\
		int(*)[1]: var,				\
		int(*)[2]: *var)

/* ONLY FOR INTERNAL USE! arr is not validated for arrayness. arr should be an array. */
#define UNSAFE_ARRAY_SIZE(_arr_) (sizeof(_arr_) / sizeof((_arr_)[0]) )
#define UNSAFE_ARRAY_SIZE_BYTES(_arr_) sizeof(_arr_)
#define UNSAFE_ARRAY_ELEMENT_SIZE(_arr_) sizeof((_arr_)[0])
#define UNSAFE_ARRAY_ELEMENT_TYPE(_arr_) typeof((_arr_)[0])

/* is_ptr_to_vla(_arr_ptr_)
 * Returns true if _arr_ptr_ is pointer to VLA. */
#define is_ptr_to_vla(_arr_ptr_) is_vla(*(_arr_ptr_))

/* PRINT_ARRAY_INFO helpers */
#define h_print_array_info(arr, append) if_vla_or_vla_ptr(arr, "VLA" append, "Array" append)
#define if_vla_or_vla_ptr(_arr_, t, f) if_constexpr(sizeof(_arr_) + sizeof(*(_arr_)), f, t)

/* Unsafe variant of fill_array(),
 * Should be used only with pointer to array, arguments are not checked for arrayness */
#define unsafe_fill_array(_arrp_, ...) \
    unsafe_foreach_array_ref(_arrp_, _ref_) \
        *(_ref_) = (__VA_ARGS__)


/* This needs more work, don't use */
#define for_each_bit_in_array(_array_) \
for(unsigned byte_index = 0; byte_index < P_ARRAY_SIZE(_array_); byte_index++) \
    for(unsigned bit_index = 0; bit_index < (P_ARRAY_ELEMENT_SIZE(_array_) * 8); bit_index++)

/* base macro for foreach_array_ref() */
#define foreach_array_ref_base(prefix, _arr_, _ref_ptr_name_)                           \
        for(prefix ARRAY_ELEMENT_TYPE(_arr_)                                            \
                (*const _tmp_arr_ptr_)[ARRAY_SIZE(_arr_)] = & auto_arr(_arr_),          \
                *_ref_ptr_name_ = unsafe_array_first_ref(_tmp_arr_ptr_);                \
                                                                                        \
                _ref_ptr_name_ < unsafe_array_end_ref(_tmp_arr_ptr_);                   \
                                                                                        \
                (_ref_ptr_name_)++)

/* unsafe_foreach_array_ref(), unsafe_foreach_array_const_ref():
 * unsafe iteration over pta. pta is not checked for being pointer to array. not safe for compound literals. only for internal usage.
 * @_array_ptr: pta
 * @_ref_ptr_name_: name of new pointer for iteration
 */
#define unsafe_foreach_array_ref(_array_ptr_, _ref_ptr_name_) \
    for(unsafe_make_array_first_ref(_array_ptr_, _ref_ptr_name_); _ref_ptr_name_ != unsafe_array_end_ref(_array_ptr_); _ref_ptr_name_++)

#define unsafe_foreach_array_const_ref(_array_ptr_, _ref_ptr_name_) \
    for(const unsafe_make_array_first_ref(_array_ptr_, _ref_ptr_name_); _ref_ptr_name_ != unsafe_array_end_ref(_array_ptr_); _ref_ptr_name_++)

/* base macro for foreach_array_ref_bw() */
#define foreach_array_ref_bw_base(prefix, _arr_, _ref_ptr_name_)		\
	for(prefix ARRAY_ELEMENT_TYPE(_arr_)					\
		(*const _tmp_arr_ptr_)[ARRAY_SIZE(_arr_)] = & auto_arr(_arr_),	\
		*_ref_ptr_name_ = unsafe_array_last_ref(_tmp_arr_ptr_);		\
										\
		_ref_ptr_name_ >= unsafe_array_first_ref(_tmp_arr_ptr_);	\
										\
		(_ref_ptr_name_)--)


/* Declares pointer to first array element */
#define make_array_first_ref(_array_ptr_, _ref_ptr_name_) ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = array_first_ref(_array_ptr_)
/* Declares pointer to last array element */
#define make_array_last_ref(_array_ptr_, _ref_ptr_name_) ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = array_last_ref(_array_ptr_)

/* unsafe_make_array_first_ref(): creates a pointer to first array element. pta is not checked for arrayness. only for internal usage.
 * @_array_ptr_: pta
 * @_ref_ptr_name: name of new pointer */
#define unsafe_make_array_first_ref(_array_ptr_, _ref_ptr_name_) UNSAFE_ARRAY_ELEMENT_TYPE(*(_array_ptr_)) *(_ref_ptr_name_) = &(*(_array_ptr_))[0]

/* Unsafe variants, _array_ptr_ should be a pta */
#define unsafe_array_first_ref(_array_ptr_) (&(*(_array_ptr_))[0])
#define unsafe_array_last_ref(_array_ptr_)  (&(*(_array_ptr_))[UNSAFE_ARRAY_SIZE(*(_array_ptr_)) - 1])
#define unsafe_array_end_ref(_array_ptr_)   (&(*(_array_ptr_))[UNSAFE_ARRAY_SIZE(*(_array_ptr_))])

/* Unsafe variants, _array_ptr_ should be a pta */
#define unsafe_is_first_array_ref(_arr_ptr_, _ref_) ((_ref_) == unsafe_array_first_ref((_arr_ptr_)))
#define unsafe_is_last_array_ref(_arr_ptr_, _ref_)  ((_ref_) == unsafe_array_last_ref((_arr_ptr_)))
#define unsafe_is_end_array_ref(_arr_ptr_, _ref_)   ((_ref_) == unsafe_array_end_ref((_arr_ptr_)))

/* copy_array() implementation */
#define unsafe_copy_array(_arrp_dst_, _arrp_src_) do {								\
	unsafe_nc_make_arrview_first(_tmp_dst_, h_copy_arr_min_size(_arrp_dst_, _arrp_src_), _arrp_dst_);	\
														\
	_dummy_type_ _same_[1 + unsafe_is_ptas_of_same_types(_tmp_dst_, _arrp_src_)];				\
														\
	if(is_dummy_true(_same_)) {										\
		memcpy(_tmp_dst_, _arrp_src_, UNSAFE_ARRAY_SIZE_BYTES(*_tmp_dst_));				\
	} else {												\
		__typeof__( if_dummy_true(_same_, &(char[]){0}, _tmp_dst_)) _s_dst_ = (void*)_tmp_dst_;		\
		__typeof__( if_dummy_true(_same_, &(const char[]){0}, _arrp_src_)) _s_src_ = (void*)_arrp_src_;	\
														\
		const unsafe_make_array_first_ref(_s_src_, _src_ref_);						\
		unsafe_foreach_array_ref(_s_dst_, _dst_ref_)							\
		    *_dst_ref_ = *_src_ref_++;									\
	}													\
} while(0)

#define h_copy_arr_min_size(_arrp_dst_, _arrp_src_) \
	h_copy_min(UNSAFE_ARRAY_SIZE(*_arrp_dst_), UNSAFE_ARRAY_SIZE(*_arrp_src_))

#define h_copy_min(a, b) a > b ? b : a

/* is_arrays_of_same_types(dst_ptr, src_ptr)
 * checks if two pointers to arrays contain same type, ignoring type constness
 *
 * char a[1];
 * const char b[1];
 * int c[1];
 *
 * is_arrays_of_same_types(&a, &a) // true
 * is_arrays_of_same_types(&a, &b) // true
 * is_arrays_of_same_types(&b, &b) // true
 * is_arrays_of_same_types(&a, &c) // false
 *
 * Three-level check for same type:
 * at first level we return true if types are exactly same (const dst == const src) or (non-const dst == non-const src)
 * at second level we return true if dst type is const and src type is non const
 * at third level we return true if dst type is non const and src type is const
 */
#define is_arrays_of_same_types(dst_ptr, src_ptr) unsafe_is_ptas_of_same_types(& auto_arr(dst_ptr), & auto_arr(src_ptr))

#define unsafe_is_ptas_of_same_types(dst_ptr, src_ptr)                           \
        _Generic((dst_ptr),                                                     \
                typeof((*(src_ptr))[0]) (*)[]: true,                            \
                default: _Generic( (dst_ptr),                                   \
                        const typeof((*(src_ptr))[0]) (*)[]: true,              \
                        default: _Generic( (src_ptr),                           \
                                const typeof((*(dst_ptr))[0]) (*)[]: true,	\
                                default: false                                  \
                                )                                               \
                        )                                                       \
                )

/* copy_arrays() implementation */
#define h_copy_arrs(_arrm_dst_, _prev_, _arrm_src_) (								\
	(void)h_copy_arrs_chk_type_sel(_arrm_dst_, _arrm_src_),							\
	(unsigned char*)memcpy(_prev_, _arrm_src_, ARRAY_SIZE_BYTES(_arrm_src_)) + ARRAY_SIZE_BYTES(_arrm_src_)	\
)

#define h_copy_arrs_chk_type_sel(_arrm_dst_, _arrm_src_) \
	POOR_ARR_CHK_SEL(h_copy_arrs_chk_type_none, h_copy_arrs_chk_type_static, h_copy_arrs_chk_type_static)(_arrm_dst_, _arrm_src_)

#define h_copy_arrs_chk_type_none(...) 0
#define h_copy_arrs_chk_type_static(_arrm_dst_, _arrm_src_)			\
	static_assert_expr(is_arrays_of_same_types(_arrm_dst_ , _arrm_src_),	\
	"copy_arrays(): source array (" #_arrm_src_ ") doesn't have same type as destination array (" #_arrm_dst_ ")")

#define h_copy_arrs_chk_size_sel(_arrm_dst_, ...) \
	POOR_ARR_CHK_SEL(h_copy_arrs_chk_size_none, h_copy_arrs_chk_size_static, h_copy_arrs_chk_size_dyn)(_arrm_dst_, __VA_ARGS__)

#define h_copy_arrs_chk_size_none(...) 0
#define h_copy_arrs_chk_size_static(_arrm_dst_, ...)					\
	ARR_ASSERT(ARRAY_SIZE(_arrm_dst_) >= ARRAYS_SIZE(__VA_ARGS__))

#define h_copy_arrs_chk_size_dyn(_arrm_dst_, ...)					\
	ARR_ASSERT_MSG(ARRAY_SIZE(_arrm_dst_) >= ARRAYS_SIZE(__VA_ARGS__),		\
		"copy_arrays(): Array \"" #_arrm_dst_ "\" has incufficient space"	\
		" (", ARRAY_SIZE(_arrm_dst_), " element(s))"				\
		" while size of all source arrays is ", ARRAYS_SIZE(__VA_ARGS__),	\
		" at " FILE_AND_LINE)							\


/***** Arrview helpers *****/
/* arrview() implemenmtation */
#define make_unsafe_arrview(_name_, _idx_, _size_, _arrp_) \
	unsafe_make_arrptr(_name_, _size_, _arrp_) = (typeof(_name_))h_unsafe_arrview(_idx_, _size_, _arrp_, "make_arrview()")

#define unsafe_arrview(_idx_, _size_, _arrp_)  (unsafe_make_arrptr(, _size_, _arrp_))h_unsafe_arrview(_idx_, _size_, _arrp_, "arrview()")

#define h_unsafe_arrview(_idx_, _size_, _arrp_, _macro_name_) \
	&(*_arrp_)[_idx_ + h_av_chk_sel(_arrp_, _idx_, _size_, _macro_name_)]

#define h_av_chk_sel(_arrp_, _idx_, _size_, _macro_name_) \
	POOR_ARR_CHK_SEL(h_av_chk_none, h_av_chk_static, h_av_chk_dyn)(_arrp_, _idx_, _size_, _macro_name_)

#define h_av_chk_none(...) 0
#define h_av_chk_static(_arrp_, _idx_, _size_, _macro_name_) _Generic(1,	\
	int*:	ARR_ASSERT(_idx_ >= 0),						\
	int**:	ARR_ASSERT(_size_ > 0),						\
	int***:	ARR_ASSERT(_idx_ + _size_ <= UNSAFE_ARRAY_SIZE(*_arrp_)),	\
	default: 0 )

#define h_av_chk_dyn(_arrp_, _idx_, _size_, _macro_name_) ((			\
	ARR_ASSERT_MSG(_idx_ >= 0,						\
		CRED _macro_name_ ": Start index ", _idx_ ," is less than 0"	\
		" (start index:", _idx_, ")"					\
		" at " FILE_AND_LINE CRESET),					\
	ARR_ASSERT_MSG(_size_ > 0,						\
		CRED _macro_name_ ": Size of the view should be greater than 0"	\
		" (size:", _size_, ")"						\
		" at " FILE_AND_LINE CRESET),					\
	ARR_ASSERT_MSG(_idx_ + _size_ <= UNSAFE_ARRAY_SIZE(*_arrp_),		\
		CRED _macro_name_ ": Out of bound view "			\
		" (start index:", _idx_, " view size:", _size_,			\
		" source array size:", UNSAFE_ARRAY_SIZE(*_arrp_), ")"		\
		" at " FILE_AND_LINE CRESET)					\
    ), 0)

/* arrview_first() implemenetation */
#define unsafe_make_arrview_first(_name_, _size_, _arrp_) \
	unsafe_make_arrptr(_name_, _size_, _arrp_) = (typeof(_name_))h_unsafe_arrview_first(_size_, _arrp_, "make_arrview_first()")

#define unsafe_arrview_first(_size_, _arrp_) (unsafe_make_arrptr(, _size_, _arrp_))h_unsafe_arrview_first(_size_, _arrp_, "arrview_first()")

#define h_unsafe_arrview_first(_size_, _arrp_, _macro_name_) &(*_arrp_)[h_av_size_chk_sel(_arrp_, _size_, _macro_name_)]

/* unsafe_nc_make_arrview_first(): same as make_arrview_first without checks at all */
#define unsafe_nc_make_arrview_first(_name_, _size_, _arrp_) \
	unsafe_make_arrptr(_name_, _size_, _arrp_) = (unsafe_make_arrptr(, _size_, _arrp_))&(*_arrp_)[0]

/* arrview_last() implementation */
#define unsafe_make_arrview_last(_name_, _size_, _arrp_) \
	unsafe_make_arrptr(_name_, _size_, _arrp_) =  (typeof(_name_))h_unsafe_arrview_last(_size_, _arrp_, "make_arrview_last()")

#define unsafe_arrview_last(_size_, _arrp_) (unsafe_make_arrptr(, _size_, _arrp_))h_unsafe_arrview_last(_size_, _arrp_, "arrview_last()")

#define h_unsafe_arrview_last(_size_, _arrp_, _macro_name_) \
	&(*_arrp_)[UNSAFE_ARRAY_SIZE(*_arrp_) - _size_ + h_av_size_chk_sel(_arrp_, _size_, _macro_name_)]

/* arrview first/last checks */
#define h_av_size_chk_sel(_arrp_, _size_, _macro_name_) \
	POOR_ARR_CHK_SEL(h_av_size_chk_none, h_av_size_chk_static, h_av_size_chk_dyn)(_arrp_, _size_, _macro_name_)

#define h_av_size_chk_none(...) 0
#define h_av_size_chk_static(_arrp_, _size_, _macro_name_) _Generic(1,		\
	int*:  ARR_ASSERT(_size_ > 0),						\
	int**: ARR_ASSERT(_size_ <= UNSAFE_ARRAY_SIZE(*_arrp_)),		\
	default: 0 )

#define h_av_size_chk_dyn(_arrp_, _size_, _macro_name_) ((					\
	ARR_ASSERT_MSG(_size_ > 0,								\
		CRED _macro_name_ ": Size of the view should be greater than 0"			\
		" (view size:", _size_, ")"							\
		" at " FILE_AND_LINE CRESET),							\
	ARR_ASSERT_MSG(_size_ <= UNSAFE_ARRAY_SIZE(*_arrp_),					\
		CRED _macro_name_ ": Out of bound view "					\
		" (view size:", _size_, " source array size:", UNSAFE_ARRAY_SIZE(*_arrp_), ")"	\
		" at " FILE_AND_LINE CRESET)							\
    ), 0)

/* arrview_shrink() implementation */
#define unsafe_make_arrview_shrink(_name_, _skip_start_, _skip_end_, _arrp_)	\
	h_av_shrnk_decl(_name_, _skip_start_, _skip_end_, _arrp_) =		\
	(typeof(_name_))h_unsafe_arrview_shrink(_skip_start_, _skip_end_, _arrp_, "make_arrview_shrink()")

#define unsafe_arrview_shrink(_skip_start_, _skip_end_, _arrp_) \
	(h_av_shrnk_decl(,_skip_start_, _skip_end_, _arrp_))h_unsafe_arrview_shrink(_skip_start_, _skip_end_, _arrp_, "arrview_shrink()")

#define h_unsafe_arrview_shrink(_skip_start_, _skip_end_, _arrp_, _macro_name_)	\
	&(*_arrp_)[_skip_start_ + h_av_shrink_chk_sel(_arrp_, _skip_start_, _skip_end_, _macro_name_)]

/* arrview shrink args check macros */
#define h_av_shrnk_decl(_name_, _skip_start_, _skip_end_, _arrp_) unsafe_make_arrptr(_name_, h_av_shrink_size(_arrp_, _skip_start_, _skip_end_), _arrp_)
#define h_av_shrink_size(_arrp_, _skip_start_, _skip_end_) UNSAFE_ARRAY_SIZE(*_arrp_) - _skip_start_ - _skip_end_

#define h_av_shrink_chk_sel(_arrp_, _skip_start_, _skip_end_, _macro_name_) \
	POOR_ARR_CHK_SEL(h_av_shrink_chk_none, h_av_shrink_chk_static, h_av_shrink_chk_dyn)(_arrp_, _skip_start_, _skip_end_, _macro_name_)

#define h_av_shrink_chk_none(_arrp_, _skip_start_, _skip_end_, _macro_name_) 0

#define h_av_shrink_chk_static(_arrp_, _skip_start_, _skip_end_, _macro_name_) _Generic(1,	\
	int*:   ARR_ASSERT(_skip_start_ >= 0 && _skip_end_ >= 0),				\
	int**:	ARR_ASSERT(_skip_start_ + _skip_end_ < UNSAFE_ARRAY_SIZE(*_arrp_)),		\
	default: 0										\
	)

#define h_av_shrink_chk_dyn(_arrp_, _skip_start_, _skip_end_, _macro_name_) ((					\
	ARR_ASSERT_MSG(_skip_start_ >= 0 && _skip_end_ >= 0,							\
		CRED _macro_name_ ": Skipping negative amount of elements"					\
		" (skipped front:", _skip_start_, " skipped back:", _skip_end_, ")"				\
		" at " FILE_AND_LINE CRESET),									\
	ARR_ASSERT_MSG(_skip_start_ + _skip_end_ < UNSAFE_ARRAY_SIZE(*_arrp_),					\
		CRED _macro_name_ ": Skipping the whole array"							\
		" (skipped front:", _skip_start_, " skipped back:", _skip_end_, ","				\
		" array size:", UNSAFE_ARRAY_SIZE(*_arrp_), ")"							\
		" at " FILE_AND_LINE CRESET)									\
	), 0)

/* arrview cfront() implementation */
#define unsafe_make_arrview_cfront(_name_, _skip_start_, _arrp_) \
	h_av_skip_decl(_name_, _skip_start_, _arrp_) = (typeof(_name_))h_unsafe_arrview_cfront(_skip_start_, _arrp_, "make_arrview_cfront()")

#define unsafe_arrview_cfront(_skip_start_, _arrp_) \
	(h_av_skip_decl(, _skip_start_, _arrp_))h_unsafe_arrview_cfront(_skip_start_, _arrp_, "arrview_cfront()")

#define h_unsafe_arrview_cfront(_skip_start_, _arrp_, _macro_name_) \
	&(*_arrp_)[_skip_start_ + h_av_skip_chk_sel(_arrp_, _skip_start_, _macro_name_)]

/* arrview_cback() implementation */
#define unsafe_make_arrview_cback(_name_, _skip_end_, _arrp_)	\
	h_av_skip_decl(_name_, _skip_end_, _arrp_) = (typeof(_name_))h_unsafe_arrview_cback(_skip_end_, _arrp_, "make_arrview_cback()")

#define unsafe_arrview_cback(_skip_end_, _arrp_) \
	(h_av_skip_decl(, _skip_end_, _arrp_))h_unsafe_arrview_cback(_skip_end_, _arrp_, "arrview_cback()")

#define h_unsafe_arrview_cback(_skip_end_, _arrp_, _macro_name_) \
	&(*_arrp_)[h_av_skip_chk_sel(_arrp_, _skip_end_, _macro_name_)]

/* Arrview cut back/front common declaration and argument checking macros */
#define h_av_skip_decl(_name_, _skip_, _arrp_) unsafe_make_arrptr(_name_, h_av_skip_size(_arrp_, _skip_), _arrp_)
#define h_av_skip_size(_arrp_, _skip_) UNSAFE_ARRAY_SIZE(*_arrp_) - _skip_

#define h_av_skip_chk_sel(_arrp_, _skip_, _macro_name_) \
	POOR_ARR_CHK_SEL(h_av_skip_chk_none, h_av_skip_chk_static, h_av_skip_chk_dyn)(_arrp_, _skip_, _macro_name_)

#define h_av_skip_chk_none(_arrp_, _skip_, _macro_name_) 0

#define h_av_skip_chk_static(_arrp_, _skip_, _macro_name_) _Generic(1,	\
	int*:  ARR_ASSERT(_skip_ >= 0),					\
	int**: ARR_ASSERT(_skip_ < UNSAFE_ARRAY_SIZE(*_arrp_)),		\
	default: 0							\
	)

#define h_av_skip_chk_dyn(_arrp_, _skip_, _macro_name_) ((					\
	ARR_ASSERT_MSG(_skip_ >= 0,								\
		CRED _macro_name_ ": Skipping negative amount of elements"			\
		" (skipped:", _skip_, ") at " FILE_AND_LINE CRESET),				\
	ARR_ASSERT_MSG(_skip_ < UNSAFE_ARRAY_SIZE(*_arrp_),					\
		CRED _macro_name_ ": Skipping the whole array"					\
		" (skipped:", _skip_,", array size:", UNSAFE_ARRAY_SIZE(*_arrp_), ")"		\
		" at " FILE_AND_LINE CRESET)							\
	), 0)

/* arrview_dim() implementation */
#define h_make_arrview_dim(_name_, _size_, _arrm_) \
	h_av_dim_decl(_name_, _size_, &auto_arr(_arrm_)) = \
	((typeof(_name_))_arrm_ + h_av_dim_chk_sel(&auto_arr(_arrm_), _size_, "make_arrview_dim()"))

#define h_arrview_dim(_size_, _arrm_) \
	(h_av_dim_decl(, _size_, &auto_arr(_arrm_)))(_arrm_ + h_av_dim_chk_sel(&auto_arr(_arrm_), _size_, "arrview_dim()"))

#define h_av_dim_decl(_name_, _size_, _arrp_) UNSAFE_ARRAY_ELEMENT_TYPE(*_arrp_)(*_name_)[ UNSAFE_ARRAY_SIZE(*_arrp_) / _size_ ][_size_]

#define h_av_dim_chk_sel(_arrp_, _size_, _macro_name_) \
	POOR_ARR_CHK_SEL(h_av_dim_chk_none, h_av_dim_chk_static, h_av_dim_chk_dyn)(_arrp_, _size_, _macro_name_)

#define h_av_dim_chk_none(...) 0
#define h_av_dim_chk_static(_arrp_, _size_, _macro_name_) _Generic(1,		\
	int*:  ARR_ASSERT(_size_ > 0),						\
	int**: ARR_ASSERT(_size_ <= UNSAFE_ARRAY_SIZE(*_arrp_)),		\
	default: 0 )

#define h_av_dim_chk_dyn(_arrp_, _size_, _macro_name_) ((						\
	ARR_ASSERT_MSG(_size_ > 0,									\
		CRED _macro_name_ ": Size of a new dimension should be greater than 0"			\
		" (dimension size:", _size_, ")"								\
		" at " FILE_AND_LINE CRESET),								\
	ARR_ASSERT_MSG(_size_ <= UNSAFE_ARRAY_SIZE(*_arrp_),						\
		CRED _macro_name_ ": Size of a new dimension is less than source array size"		\
		" (dimension size:", _size_, " source array size:", UNSAFE_ARRAY_SIZE(*_arrp_), ")"	\
		" at " FILE_AND_LINE CRESET)								\
    ), 0)

/* arrview_flat() implementation */
#define h_make_arrview_flat(_name_, _arrm_) h_av_flat_decl(_name_, &auto_arr(_arrm_)) = (typeof(_name_))_arrm_
#define h_arrview_flat(_arrm_) (h_av_flat_decl(, &auto_arr(_arrm_)))_arrm_

#define h_av_flat_decl(_name_, _arrp_) \
	UNSAFE_ARRAY_ELEMENT_TYPE((*_arrp_)[0])(*_name_) [UNSAFE_ARRAY_SIZE(*_arrp_) * UNSAFE_ARRAY_SIZE((*_arrp_)[0])]

/* declare_string_literal(name, string_literal)
 *
 * Only declares constant pointer to array of constant chars
 * can be used with extern.
 * example:
 *
    //Export string literal from library

//  library.h:
    #define SUPER_STRING "This is string!"
    extern declare_string_literal(super_string, SUPER_STRING);

//  library.c:
    #include "library.h"
    string_literal(super_string, SUPER_STRING);

//  main.c:
    #include "library.h"

    int main() {
        println(*super_string);

        make_array_slice_string(ss, super_string);
        print_array(ss);

        PRINT_ARRAY_INFO(super_string);

        return 0;
    }
 *
 */
#define declare_string_literal(_name_, _string_) \
    const typeof( (_string_)[0] ) (*const (_name_)) [ is_same_type( array_first_ref(&(_string_)), char*, 1, 0) ? ARRAY_SIZE(_string_) : -1 ]


/* Just prints array without any formatting.
 * Type of array elements should be one of standard C types. See println() for more info.
 * @__VA_ARGS__: array or pointer to an array
 * example:

    print_array_raw(&(int[]){1, 2, 3, 4, 5, 6}); //will print: 123456

    bool sexy[] = {true, false, true, true};
    print_array_raw(&sexy); //will print: truefalsetruetrue

    print_array_raw(&"String is an array too!"); //will print that string. less effective than println(), but works =)
 */
#define print_array_raw(...) do {		\
	foreach_array_ref((__VA_ARGS__), ref) {	\
		print(*ref);			\
	}					\
} while (0)

/* Prints array as formatted output with custom print function for each element
 * Useful for printing arrays with custom types or multidimensional arrays.
 * @__VA_ARGS__: array or pointer to an array to print
 * @fmt_fn: function or function-like macro.
 *   should accept single argument: pointer to constant element of the array to be printed.
 *
 * example:

	// Printing an array of structs
	struct toto {
		int a;
		char *b;
	};
	struct toto some_data[4] = {
		{1, "First"},
		{2, "Second"},
		{3, "Third"},
		{4, "Fourth"},
	};

	#define toto_print(ptr) print("{a:", ptr->a, " b:", ptr->b, "}")

	print_array_fmt(toto_print, some_data); //prints [{a:1 b:First},{a:2 b:Second},{a:3 b:Third},{a:4 b:Fourth}]

	//Printing a multi-dimensional array using inline function

	static inline void print_somedata(const int (*data)[4]) {
		print("\n\t");
		print_array(data);
	}

	int data[2][4] = {
		{1,2,3,4},
		{5,6,7,8},
	};

	print_array_fmt(print_somedata, data);
	//prints:
	//[
	//    [1,2,3,4]
	//,
	//    [5,6,7,8]
	//]

 */
#define print_array_fmt(_fmt_fn_, ...) do {					\
	const make_arrview_full(_tmp_arr_ptr_, __VA_ARGS__);			\
	unsafe_make_array_first_ref(_tmp_arr_ptr_, _ref_);			\
										\
	print((char)'['); _fmt_fn_(_ref_);					\
	for(_ref_++; _ref_ != unsafe_array_end_ref(_tmp_arr_ptr_); _ref_++) {	\
		print((char)','); _fmt_fn_(_ref_);				\
	}									\
	println("]");								\
} while (0)

/* array_inseret_array() implementation */
#define h_array_insert_array(_arr_, _idx_, _src_arr_) do {                                                             \
    make_arrview(_to_replace_, _idx_, ARRAY_SIZE(_src_arr_), _arr_);                                                   \
    memmove(unsafe_array_end_ref(_to_replace_),                                                                        \
            unsafe_array_first_ref(_to_replace_),                                                                      \
            (array_last_ref(_arr_) - unsafe_array_last_ref(_to_replace_)) * UNSAFE_ARRAY_ELEMENT_SIZE(*_to_replace_)); \
    copy_array(_to_replace_, _src_arr_);                                                                               \
} while (0)

/* Array bit operations. Experimental */

#define ARRAY_SIZE_BITS(arr) (ARRAYS_SIZE_BYTES(arr) * 8)

#define array_bit_idx_to_byte_idx(arr, idx) ((idx) / (ARRAY_ELEMENT_SIZE(arr) * 8))

#define _array_bit_chk_index(arr, idx, ...) _Generic(1,             \
    int*:  sizeof(char [idx < 0 ? -1 : 1]),                         \
    int**:  sizeof(char [idx >= ARRAY_SIZE_BITS(arr) ? -1 : 1]),    \
    default: __VA_ARGS__                                            \
    )

#define array_set_bit(arr, idx) \
    _array_bit_chk_index(arr, idx,  \
        auto_arr(arr)[ (idx) / (ARRAY_ELEMENT_SIZE(arr) * 8ULL) ] |= (1ULL << (idx % (ARRAY_ELEMENT_SIZE(arr) * 8ULL))) \
    )

#define array_unset_bit(arr, idx)   \
    _array_bit_chk_index(arr, idx,  \
        auto_arr(arr)[ (idx) / (ARRAY_ELEMENT_SIZE(arr) * 8ULL) ] &= ~(1ULL << (idx % (ARRAY_ELEMENT_SIZE(arr) * 8ULL))) \
    )

#define array_get_bit(arr, idx) \
    _array_bit_chk_index(arr, idx,  \
        ((bool)(!!(auto_arr(arr)[ (idx) / (ARRAY_ELEMENT_SIZE(arr) * 8ULL) ] & (1ULL << (idx % (ARRAY_ELEMENT_SIZE(arr) * 8ULL)))))) \
    )

#define foreach_array_bit(arr, _bit_idx_name_) \
    for(size_t _bit_idx_name_ = 0;             \
        _bit_idx_name_ < ARRAY_SIZE_BITS(arr); \
        _bit_idx_name_ ++)

#define foreach_array_bit_bw(arr, _bit_idx_name_)           \
    for(size_t _bit_idx_name_ = ARRAY_SIZE_BITS(arr) - 1;   \
        _bit_idx_name_ < ARRAY_SIZE_BITS(arr); \
        _bit_idx_name_ --)

#define print_array_bits(...) do {						\
	make_arrview_full(_arr_, __VA_ARGS__);					\
	print((char)'|');							\
										\
	const size_t _tmp_el_size_ = UNSAFE_ARRAY_ELEMENT_SIZE(*_arr_);		\
	foreach_array_bit_bw(_arr_, _bit_idx_) {				\
		print( array_get_bit(_arr_, _bit_idx_) ? CGREEN "X" : CRED "O");\
										\
		if(!(_bit_idx_ % (_tmp_el_size_ * 8)))				\
			print(CRESET "|");					\
		else if( !(_bit_idx_ % 8) )					\
			print(CBLUE "|");					\
	}									\
	print((char)'\n');							\
										\
	const size_t _tmp_arr_size_ = UNSAFE_ARRAY_SIZE_BYTES(*_arr_);		\
	for(size_t _byte_idx_ = _tmp_arr_size_; _byte_idx_ <= _tmp_arr_size_; _byte_idx_--) {	\
		print(!(_byte_idx_ % _tmp_el_size_) ? CRESET : CBLUE,				\
			fmt_w( _byte_idx_ ? (_byte_idx_ * 8) - 1 : 0, _byte_idx_ ? -9 : 0));	\
	}											\
	print((char)'\n');									\
} while(0)

/* Backward compatibility */

#define P_ARRAY_SIZE(arr) ARRAY_SIZE(arr)
#define P_ARRAY_SIZE_BYTES(arr) ARRAY_SIZE_BYTES(arr)
#define P_ARRAY_ELEMENT_SIZE(arr) ARRAY_ELEMENT_SIZE(arr)
#define P_ARRAY_ELEMENT_TYPE(arr) ARRAY_ELEMENT_TYPE(arr)

#define P_ARRAYS_SIZE(...) ARRAYS_SIZE((__VA_ARGS__))
#define P_ARRAYS_SIZE_BYTES(...) ARRAYS_SIZE_BYTES((__VA_ARGS__))

/* Obsolete arrslice */
#define make_array_slice_size(_name_, start, size, ...) make_arrview(_name_, start, size, __VA_ARGS__)
#define array_slice_size(start, size, ...) arrview(start, size, __VA_ARGS__)

#define make_array_slice_first(_name_, size, ...) make_arrview_first(_name_, size, __VA_ARGS__)
#define array_slice_first(size, ...) arrview_first(size, __VA_ARGS__)

#define make_array_slice_last(_name_, size, ...) make_arrview_last(_name_, size, __VA_ARGS__)
#define array_slice_last(size, ...) arrview_last(size, __VA_ARGS__)

#define make_array_slice_shrink(_name_, skip_start, skip_end, ...) make_arrview_shrink(_name_, skip_start, skip_end, __VA_ARGS__)
#define array_slice_shrink(skip_start, skip_end, ...) arrview_shrink(skip_start, skip_end, __VA_ARGS__)

#define make_array_slice_front(_name_, start, ...) make_arrview_cfront(_name_, start, __VA_ARGS__)
#define array_slice_front(start, ...) arrview_cfront(start, __VA_ARGS__)

#define make_array_slice_back(_name_, end, ...) make_arrview_cback(_name_, end, __VA_ARGS__)
#define array_slice_back(end, ...) arrview_cback(end, __VA_ARGS__)

#define make_array_slice_full(_name_, ...) make_arrview_full(_name_, __VA_ARGS__)
#define array_slice_full(...) arrview_full(__VA_ARGS__)

#define make_array_slice_string(_name_, ...) make_arrview_str(_name_, __VA_ARGS__)
#define array_slice_string(...) arrview_str(__VA_ARGS__)


/* Obsolete arrayr ref remove */
#define array_ref_remove(_arr_ptr_, _ref_, _val_)  array_remove_ref_fill(_arr_ptr_, _ref_, _val_)

#endif //POOR_ARRAY_H
