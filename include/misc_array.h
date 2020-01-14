#ifndef MISC_ARRAY_H
#define MISC_ARRAY_H

#include <misc.h>
#include <inttypes.h>

/***** Experimental *****/

/* returns pointer to int if (expr) is constant integer expression, or returns pointer to void if (expr) is not constant integer expression */
#define magic_ice_expression(expr) (1 ? ((void *)((intptr_t)( (expr) ) * 0)) : (int *)1)

/* returns true if (expr) is constant integer expression, or false if it is not */
#define is_const_expr(expr) _Generic( magic_ice_expression(expr), int*: true, void*:false)

/* Evaluates (if_const) expression if (expr) is constant integer expression or evaluates (if_not_const) expression */
#define if_constexpr(expr, if_const, if_not_const) _Generic( magic_ice_expression(expr), int*: (if_const), void*:(if_not_const))


/* This macro returns (var) as is if size of (var) or (*var) is known at compile time.
 * If sizeof(var) or sizeof(*var) is not compile time constants, then
 * macro returns fixed array or pointer to a fixed array of _dummy_type_ as a compound literal
 *
 * This is a helper macro for using together with typeof inside _Generic selectors.
 *
 * According to C18 Standard, this code is invalid:

  int size = 0;
  char (*a)[size];
  _Generic(&a, char (*)[size]: 1, default: 0)

 * Because char(*)[size] is a pointer to VLA, and no VLA can be used as type selectors inside _Generic()
 *
 * If we rewrite _Generic() used above and use typeof() as selector, then this will be:

   _Generic(&a, typeof(*a)(*)[]: 1, default: 0)

 * in this case, because (a) is a pointer to array, then typeof(*a) unflods to char[size], which is VLA
 * By using unvla() macro we can replace VLA with dummy fixed array.
 *
   _Generic(&a, typeof(*unvla(a))(*)[]: 1, default: 0)

 * here, typeof(*unvla(a)) unflods to union _dummy_type_(*)[1], which is not VLA, and default selector succesfully evaluates and returns 0
 *
 */
#define unvla(var) if_constexpr(sizeof(var),                                            \
                                 if_constexpr(sizeof(*(var)),                           \
                                                var, (union _dummy_type_ (*)[1]){0} ),  \
                                 (union _dummy_type_ [1]){0})
union _dummy_type_ {char a;};

/* This macro returns (var) if (var) is an array or returns (*var) if var is a pointer to something */
#define get_array_or_deref(var)                         \
        _Generic( &( (typeof( unvla(var) )){0} ),       \
        typeof( *(unvla(var)) ) (*)[] : (var),          \
        default: *(var))


/* auto_arr(var)
 * @var: array or pointer to an array
 *
 * This macro returns (var) if (var) is an array, or (*var) if (var) is a pointer to an array
 * Basically, this macro automatically dereferences pointer to arrays, which can be used for other functions
 *
 * example:

    char str[] = "String";
    char (*str_ptr) [ARRAY_SIZE(str)] = &str;

    //These two operations are the same:
    auto_arr(str)[1] = 'p';
    auto_arr(str_ptr)[1] = 'r';

    //Works with multi-dimensional arrays too:
    char   array [2][5][4] = {0};
    char (*arptr)[2][5][4] = &array;

    auto_arr(array)[0][0][1] = 'R';
    auto_arr(arptr)[0][1][0] = 'F';

    println( auto_arr(arptr)[0][0][1] ); //Prints 'R'
    println( auto_arr(array)[0][1][0] ); //Prints 'F'

NOTE: Only works for multi-dimensional VLA with last dimension being variable, eg:
        char array [ number ][2][3];        //OK
        char array [ number ][ number ][3]; //Fail
        char array [1][ number ][3];        //Fail
 */
#define auto_arr(var)                                   \
         _Generic( &( get_array_or_deref(var) ),        \
        typeof( *(get_array_or_deref(var)) ) (*)[]: get_array_or_deref(var))


/* Macro alias for auto_arr() */
#if !defined(arr)
#define arr(arr) auto_arr((arr))
#endif
/***** end experimental *****/

/* Returns number of elements in the array */
#define ARRAY_SIZE(arr) ( sizeof( auto_arr(arr)) / sizeof (auto_arr(arr)[0]) )
/* Returns total array size in bytes */
#define ARRAY_SIZE_BYTES(arr) sizeof( auto_arr(arr) )
/* Returns size of one element in the array */
#define ARRAY_ELEMENT_SIZE(arr) sizeof( auto_arr(arr)[0] )
/* Extracts type of array element from array */
#define ARRAY_ELEMENT_TYPE(arr) typeof( auto_arr(arr)[0])

/* Get sum of array sizes
 * @__VA_ARGS__: arrays or pointer to arrays
 * example:

    char te[] = {1, 2, 3};
    long (*me)[2] = &(long[]){5, 6};
    println(ARRAYS_SIZE(te, me)); //prints: 5
*/
#define ARRAYS_SIZE(...) ( MAP(array_size_helper__, __VA_ARGS__) 0 )
#define array_size_helper__(arr) ARRAY_SIZE(arr) +

/* Get total size in bytes for all arrays
 * @__VA_ARGS__: arrays or pointer to arrays
 * example:

    uint8_t te[] = {1, 2, 3};
    long (*me)[2] = &(long[]){5, 6};
    println(ARRAYS_SIZE_BYTES(te, me)); //prints: 19  (if sizeof(long) == 8, then (8 * 2) + 3 )
 */
#define ARRAYS_SIZE_BYTES(...) ( MAP(array_size_bytes_helper__, __VA_ARGS__) 0 )
#define array_size_bytes_helper__(arr) ARRAY_SIZE_BYTES(arr) +

/* is_ptr_to_vla(_arr_ptr_)
 * Returns true if _arr_ptr_ is pointer to VLA. */
#define is_ptr_to_vla(_arr_ptr_) is_vla(*(_arr_ptr_))

/* is_vla(array)
 * Returns true if array is variable length array(VLA) */
#define is_vla(arr) (!is_const_expr(sizeof(arr)))

/* PRINT_ARRAY_INFO(arr_ptr): Prints information about array
 * @__VA_ARGS__: pointer to an array
 * example:

    PRINT_ARRAY_INFO(&(long long[]){1, 2, 3, 4});
    //prints: Array "&(long long[]){1, 2, 3, 4}" at 0x7fffffffdc40 has size:4 uses 32 bytes, while single element has size:8

...
int main(int argc, char **argv) {
    short (*test)[argc * 3];
    malloc_array(test);

    PRINT_ARRAY_INFO(test);
    //if argc == 1, prints: VLA "test" at 0x5555555592a0 has size:3 uses 6 bytes, while single element has size:2

    free(test);
    return 0;
}

 */
#define PRINT_ARRAY_INFO(...) println(is_vla( auto_arr(__VA_ARGS__)) ? "VLA" : "Array" , " \"" #__VA_ARGS__ "\" at ", ((const void*)(__VA_ARGS__)) ,	\
                                     " has size:", P_ARRAY_SIZE((__VA_ARGS__)),                                                                         \
                                     " uses ", P_ARRAY_SIZE_BYTES((__VA_ARGS__)), " bytes,"                                                             \
                                     " while single element has size:", P_ARRAY_ELEMENT_SIZE((__VA_ARGS__)))


/* make_array_ptr()
 *
 * Declares an array pointer and sets it's value to (pointer)
 * Size of array calculated from size of element where (pointer) points and (size) variable
 *
 * example:

    //Wraping pointer and size, returned from another function

    size_t size;
    void *ptr = function_which_returns_pointer_and_size(&size);
    if(!ptr || !size)
        return;

    make_array_ptr(data, ptr, size);

    foreach_array_const_ref(data, ref)
        println("value:", *ref);
...
    //Simple program:

int main(int argc, char**argv) {
    make_array_ptr(args, argv, argc);
    print_array(args);
    return 0;
}

 * NOTE: size should not be zero, but pointer can be NULL.
 */
#define make_array_ptr(array_pointer_name, pointer, size) typeof(*pointer) (*(array_pointer_name))[size]  = (void*)pointer

/* Allocates memory for pointer to an array.
 * Provide pointer to an array, to allocate memory. Pointer can be pointer to VLA.
 * Size of allocation will be deduced from pointer type;
 *
 * return true if allocation successful, or false on failure;
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
#define malloc_array(_arr_ptr_) (( _arr_ptr_ = malloc( P_ARRAY_SIZE_BYTES(_arr_ptr_) ) ))

/* Same as malloc_array, but with calloc */
#define calloc_array(_arr_ptr_) (( _arr_ptr_ = calloc( 1, P_ARRAY_SIZE_BYTES(_arr_ptr_) ) ))

/* Fills array with specified symbol using memset
 * example:

    char (*data)[5];
    malloc_array(data);
    memset_array(data, 'L');

    foreach_array_ref(data, ref)
        print(*ref); //Will print: LLLLL

*/
#define memset_array(_arr_ptr_, symbol) memset((_arr_ptr_), symbol, P_ARRAY_SIZE_BYTES(_arr_ptr_))

/* This needs more work, don't use */
#define for_each_bit_in_array(_array_) \
for(unsigned byte_index = 0; byte_index < P_ARRAY_SIZE(_array_); byte_index++) \
    for(unsigned bit_index = 0; bit_index < (P_ARRAY_ELEMENT_SIZE(_array_) * 8); bit_index++)

/* Iterate over an array.
 * @_arr_ptr_: array or pointer to array
 * @_ref_ptr_name_: name of pointer which will be used as reference to each array element
 * example:
 *
    uint16_t test[] = {1, 2, 3, 4};
    foreach_array_ref(&test, val)
        print(*val);  //prints: 1234

 *
 * NOTE: if _arr_ptr_ contains const data, then _ref_ptr_name_ also will be const
 */
#define foreach_array_ref(_arr_ptr_, _ref_ptr_name_) \
    for(make_array_first_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ != array_end_ref((_arr_ptr_)); (_ref_ptr_name_)++  )

/* same as foreach_array_ref(), but _ref_ptr_name_ is always const */
#define foreach_array_const_ref(_arr_ptr_, _ref_ptr_name_) \
     for(const make_array_first_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ != array_end_ref((_arr_ptr_)); (_ref_ptr_name_)++  )

/*
 * Iterate backwards over an array. Same as foreach_array_ref(), but backwards.
 * example:
 *
    uint16_t test[] = {1, 2, 3, 4};
    foreach_array_ref_bw(&test, val)
        print(*val);  //prints: 4321
 */
#define foreach_array_ref_bw(_arr_ptr_, _ref_ptr_name_) \
    for(make_array_last_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ >= array_first_ref((_arr_ptr_)); (_ref_ptr_name_)--  )

/* same as foreach_array_ref_bw(), but _ref_ptr_name_ is always const */
#define foreach_array_const_ref_bw(_arr_ptr_, _ref_ptr_name_) \
    for(const make_array_last_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ >= array_first_ref((_arr_ptr_)); (_ref_ptr_name_)--  )

/* Get index of an array element where _ref_ is pointing.
 * @_array_ptr_: array or pointer to array
 * @_ref_: pointer to an element inside that array
 * example:

    uint64_t test[] = {0x2000, 100, 200, 300};
    foreach_array_ref(&test, val) {
        println("Array index:", array_ref_index(test, val), " Array value:0x", to_hex(*val, 8));

        if(array_ref_index(test, val) == 2)
            println("Magic index 2 detected, value at index 1 is ", *(val-1) , " and value at index 3 is ", test[3]);
    }

*/
#define array_ref_index(_array_ptr_, _ref_) ((_ref_) - array_first_ref(_array_ptr_))

/* Declares pointer to first array element */
#define make_array_first_ref(_array_ptr_, _ref_ptr_name_) P_ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = array_first_ref(_array_ptr_)

/* Declares pointer to last array element */
#define make_array_last_ref(_array_ptr_, _ref_ptr_name_) P_ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = array_last_ref(_array_ptr_)

/* get pointer to first array element */
#define array_first_ref(_array_ptr_) (& auto_arr((_array_ptr_))[0])

/* get pointer to last array element */
#define array_last_ref(_array_ptr_) (& auto_arr((_array_ptr_))[ P_ARRAY_SIZE(_array_ptr_) - 1 ])

/* get pointer to the array element after the last one. While this is a valid pointer, IT SHOULD NOT BE DEREFERENCED! */
#define array_end_ref(_array_ptr_) (& auto_arr((_array_ptr_))[ P_ARRAY_SIZE(_array_ptr_) ])

/* returns true if ref points to last array element */
#define is_last_array_ref(_arr_ptr_, _ref_) ((_ref_) == array_last_ref( (_arr_ptr_) ))
/* returns true if ref points to first array element */
#define is_first_array_ref(_arr_ptr_, _ref_) ((_ref_) == array_first_ref( (_arr_ptr_) ))
/* returns true if ref point to first or last array element */
#define is_first_or_last_array_ref(_arr_ptr_, _ref_) ( is_first_array_ref((_arr_ptr_), (_ref_)) || is_last_array_ref((_arr_ptr_), (_ref_))  )


/* Erases single element from array by moving all data after _ref_ towards front
 * of the array and setting last array value with _val_
 *
 * example:

  uint8_t arr[] = {1, 2, 3, 4, 5};
  uint8_t *ref = &arr[2]; //pointer to value '3'
  array_remove_ref(&arr, ref, 50);
  //  array now will be: {1, 2, 4, 5, 50}

  ref = &(arr[4]); //pointer to last element: '50'
  array_remove_ref(&arr, ref, 0);
  //  array now will be: {1, 2, 4, 5, 0}

 * BE CAREFUL WHILE ITERATING!
 */
#define array_ref_remove(_arr_ptr_, _ref_, _val_) do {                          \
        if(is_last_array_ref((_arr_ptr_), (_ref_))) {                           \
                *(_ref_) = (_val_);                                             \
        } else {                                                                \
                memmove((_ref_), (_ref_) + 1, P_ARRAY_SIZE_BYTES(_arr_ptr_) - ((array_ref_index( (_arr_ptr_) , (_ref_)) + 1)  * P_ARRAY_ELEMENT_SIZE(_arr_ptr_) ) ); \
                *(array_last_ref(_arr_ptr_)) = (_val_);                         \
        }                                                                       \
} while (0)

/* Copies data from src array pointer to dst array pointer per element
 * if dst array is larger or equal than source array, then entire source array will be copied to dst.
 *  excess bytes in dst won't be touched.
 *
 * if dst array is smaller than src array, then only part of source will be copied from src with size equals dst array.
 *
 * dst and src arrays can be of different type. i.e. it is valid to copy data from array of ints to array of longs. But compiler may complain if loosing precision.
 *
 * @dst_ptr: pointer to destination array
 * @__VA_ARGS__: pointer to source array (we have to use __VA_ARGS__ here, because of commas in compound initializer. see below)
 *
 * examples:

    int dst[5] = {1,2,3,4,5};
    int src[3] = {9,8,7};
    copy_array(&dst, &src);
    print_array(&dst); //prints: 98745
...
    int dst[] = {1,2};
    int src[] = {9,8,7};
    copy_array(&dst, &src);
    print_array(&dst); //prints: 98
...
    //Copy string into dynamic memory
    char (*data)[ sizeof ("String lol !") ];
    malloc_array(data);
    copy_array(data, &"String lol !");
    print_array(data); //prints: String lol !
    free(data);
...
    //Just madness:

    size_t size1 = argc;
    size_t size2 = 3 * argc;

    //Create two VLAs
    int arr1[size1];
    unsigned arr2[size2];

    // 'E' - initialize them
    memset_array(&arr1, 'E');
    memset_array(&arr2, 'E');

    //Copy data from static arrays to VLAs. if VLAs will be larger than these arrays then there will be 'E'
    copy_array(&arr1, &(unsigned short[]){'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'});
    copy_array(&arr2, &(uint8_t[]){'Z', 'Y', 'X', 'W', 'V', 'U'});

    //Create new array pointer with array size (arr1 + arr2)
    char (*data)[ARRAY_SIZE(arr1) + ARRAY_SIZE(arr2)];

    //Allocate memory for it
    malloc_array(data);

    //Copy data from first VLA
    copy_array(data, &arr1);

    //Create new slice array pointer from data
    make_array_slice(data_slice, ARRAY_SIZE(arr1), P_ARRAY_SIZE(data), data);

    //Copy data from second VLA to this data_slice
    copy_array(data_slice, &arr2);

    print_array(data);
         //if argc was 1. then it will print: AZYX
         //if argc was 2. then it will print: ABZYXWVU
         //if argc was 3. then it will print: ABCZYXWVUEEE

    free(data);
 */

#define copy_array(dst_ptr, ...) do {                               \
        if(is_arrays_of_same_types(dst_ptr, (__VA_ARGS__))) {       \
            copy_array_fast__(dst_ptr, (__VA_ARGS__));              \
        } else {                                                    \
            copy_array_slow__(dst_ptr, (__VA_ARGS__));              \
        }                                                           \
} while(0)

#define copy_array_fast__(dst_ptr, src_ptr)                         \
    (P_ARRAY_SIZE_BYTES(dst_ptr) >= P_ARRAY_SIZE_BYTES(src_ptr))    \
        ? memcpy(dst_ptr, src_ptr, P_ARRAY_SIZE_BYTES(src_ptr))     \
        : memcpy(dst_ptr, src_ptr, P_ARRAY_SIZE_BYTES(dst_ptr))     \

#define copy_array_slow__(dst_ptr, src_ptr) do {                  \
    if(P_ARRAY_SIZE(dst_ptr) >= P_ARRAY_SIZE((src_ptr))) {        \
        make_array_first_ref(dst_ptr, dst_ref);                   \
        foreach_array_const_ref(src_ptr, src_ref) {               \
            *dst_ref++ = *src_ref;                                \
        }                                                         \
    } else {                                                      \
        const make_array_first_ref(src_ptr, src_ref);             \
        foreach_array_ref(dst_ptr, dst_ref) {                     \
            *dst_ref = *src_ref++;                                \
        }                                                         \
    }                                                             \
} while(0)

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
#define is_arrays_of_same_types(dst_ptr, src_ptr) is_arrays_of_same_types_raw(& auto_arr(dst_ptr), & auto_arr(src_ptr))

#define is_arrays_of_same_types_raw(dst_ptr, src_ptr)                           \
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


/* Copies multiple arrays into single array
 * @dst_ptr: pointer to destination array
 * __VA_ARGS__: pointers to source arrays to copy data from
 *
 * All source arrays must contain same element type as destination array
 * Destination array size must be equal or larger than total size of all source arrays.
 *
 * example:

    make_array_slice_string(src1, &"This ");
    make_array_slice_string(src2, &"Is ");
    make_array_slice_string(src3, &"A string");
    char dst[P_ARRAYS_SIZE(src1, src2, src3)];
    copy_arrays(&dst, src1, src2, src3);
    print_array(&dst); //prints: This Is A string

 */
#define copy_arrays(dst_ptr, ...) do {                              \
    MAP_ARG(check_source_arrays, dst_ptr, __VA_ARGS__)              \
    make_array_first_ref(dst_ptr, tmp_ptr);                         \
    (                                                               \
        MAP_LIST(copy_arrays_helper, __VA_ARGS__)                   \
    );                                                              \
} while(0)

#define check_source_arrays(dst_ptr, src_ptr) \
    static_assert( is_arrays_of_same_types(dst_ptr, src_ptr) == true, "Source array: " #src_ptr " doesn't have same type as destination array");

#define copy_arrays_helper(src_ptr) (void)(tmp_ptr = ((typeof(tmp_ptr))memcpy(tmp_ptr, src_ptr, P_ARRAY_SIZE_BYTES(src_ptr))) + P_ARRAY_SIZE(src_ptr))

/* makes array slice
 * @_name_: name of variable to declare for slice
 * @_start_: start index
 * @_end_: end index
 * @__VA_ARGS__: pointer to an array from which array slice will be made
 *
 * @start should not be less or equal @end
 * both @start and @end should not be equal or greater than source array size
 *
 * an array slice is just a pointer to array to some position inside another array.
 * basically you can write that manually:
 *
 * char arr[6] = {'a','b','c','d','e','f'};
 * char (*slice)[3] = (char (*)[3])(arr + 2); // Same as make_array_slice(slice, 2, 5, &arr),
 *                                            // or make_array_slice_size(slice, 2, 3, &arr);
 *
 * slice pointer here points to arr's element at index 2, and has size 3.
 * so, if we dereference the slice, we can access arr's values at indexes 2, 3, 4
 *
 * print_array(slice); //prints: cde
 *
 * and we can modify arr's values through the slice:
 *
 * (*slice)[0] = 'x';
 * (*slice)[1] = 'y';
 * (*slice)[2] = 'z';
 *
 * print_array(&arr); //prints: abxyzf
 *
 * we can, of course try to do that: access element above slice bounds.
 *
 * (*slice)[3] = 'f'; //NO !
 *
 * While in the example above it is fine to do that,
 * since (*slice)[3] is the same as arr[5] (value 'f'), but doing that is a bad practice
 * and also compiler may send a warning, that you trying to write to an array above it's bounds
 *
 * if you need to access slice above it's bounds, then create larger slice in the first place!
 *
 * examples:

    //make string literal slice
    make_array_slice(slice, 4, 9, &"--> HELLO <--!");
    print_array(slice); //prints: HELLO
...
    //Cut char array
    make_array_slice(slice, 1, 3, &(char[]){'>', 'H', 'I', '<'});
    print_array(slice); //prints: HI
...
    //Create concatenated VLA and cut it
    concat_vla(vla, "Int:", 1, " float:", 5.f, "\n");
    make_array_slice(slice, 6, ARRAY_SIZE(vla) - 1, &vla);
    print_array(slice); //prints: float:5.000000
...
    //Copy three string into one

    //Declare string literals
    string_literal(pre_string, "This ");
    string_literal(mid_string, "is ");
    string_literal(post_string, "text");

    // Make slices for first two strings without null terminator
    make_array_slice_string(pre, pre_string);
    make_array_slice_string(mid, mid_string);

    // Create array
    char (*string)[P_ARRAYS_SIZE(pre, mid, post_string)];
    malloc_array(string);

    //Make a slice from string with size of array pre
    make_array_slice_size(string_slice0, 0, P_ARRAY_SIZE(pre), string);
    copy_array(string_slice0, pre);

    //Make a slice from string with size of array mid and with start position after pre string
    make_array_slice_size(string_slice1, P_ARRAY_SIZE(pre), P_ARRAY_SIZE(mid), string);
    copy_array(string_slice1, mid);

    //Make a slice from string at position after pre and mid.
    make_array_slice_front(string_slice2, P_ARRAYS_SIZE(pre, mid), string);
    copy_array(string_slice2, post_string);

    println(*string);
    free(string);

    NOTE: slice array pointer will be VLA pointer if start or end computed at runtime. otherwise it will be a simple pointer.
 */
#define make_array_slice(_name_, start, end, ...) \
   { __attribute__((unused)) char end_greater_than_array_size[ (P_ARRAY_SIZE((__VA_ARGS__)) >= end ) ? 1 : -1];} \
   { __attribute__((unused)) char start_index_should_not_be_equal_end_index[ (start == end) ? -1 : 1];} \
   { __attribute__((unused)) char end_index_should_not_be_less_than_start_index[ (end < start) ? -1 : 1];} \
   P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*(_name_)) [ (end) - (start) ] = array_slice(start, end, (__VA_ARGS__))

#define array_slice(start, end, ...) (P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*)[ (end) - (start) ]) &(auto_arr((__VA_ARGS__))[start])

/* make array slice, skipping (start) elements at front */
#define make_array_slice_front(_name_, start, ...) \
   { __attribute__((unused)) char start_index_is_greater_than_array_size_minus_one [ (P_ARRAY_SIZE((__VA_ARGS__)) > start ) ? 1 : -1];} \
   P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*(_name_)) [ P_ARRAY_SIZE((__VA_ARGS__)) - (start) ] = array_slice_front(start, (__VA_ARGS__))

#define array_slice_front(start, ...) (P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*)[ P_ARRAY_SIZE((__VA_ARGS__)) - start ]) &(auto_arr((__VA_ARGS__))[start])


/* make array slice, skipping (end) elements at array end
 * example:

    //Cut '\0' from string and print it as array
    make_array_slice_back(string_view, 1, &"String is an array too!");
    print_array(string_view);

 */
#define make_array_slice_back(_name_, end, ...) \
   { __attribute__((unused)) char end_index_is_greater_than_array_size_minus_one [ (P_ARRAY_SIZE((__VA_ARGS__)) > end ) ? 1 : -1];} \
   P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*(_name_)) [ P_ARRAY_SIZE((__VA_ARGS__)) - (end) ] = array_slice_back(end, (__VA_ARGS__))

#define array_slice_back(end, ...) (P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*)[ P_ARRAY_SIZE((__VA_ARGS__)) - end ]) &(auto_arr((__VA_ARGS__))[0])


/* make array slice with size (size), skipping (start) elements at front */
#define make_array_slice_size(_name_, start, size, ...) \
   { __attribute__((unused)) char start_and_size_together_greater_than_array_size[ (P_ARRAY_SIZE((__VA_ARGS__)) >= start + size ) ? 1 : -1];} \
   { __attribute__((unused)) char size_should_not_be_zero[ size ? 1 : -1 ];  }                                       \
   P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*(_name_)) [ (size) ] = array_slice_size(start, size, (__VA_ARGS__))

#define array_slice_size(start, size, ...) (P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*)[size]) &(auto_arr((__VA_ARGS__)) [start])

/* Make array slice while skipping n bytes from start and n bytes from end
 * example:

   char data[] = {0, 0, 'H','I', 0, 0, 0};
   make_array_slice_shrink(data_slc, 2, 3, &data);
   print_array(data_slc); //prints: HI

 */
#define make_array_slice_shrink(_name_, skip_start, skip_end, ...) \
        P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*(_name_)) [ P_ARRAY_SIZE((__VA_ARGS__)) - (skip_start) - (skip_end) ] = array_slice_shrink(skip_start, skip_end, (__VA_ARGS__))

#define array_slice_shrink(skip_start, skip_end, ...) \
        (P_ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*) [ P_ARRAY_SIZE((__VA_ARGS__)) - (skip_start) - (skip_end) ]) &(auto_arr((__VA_ARGS__))[skip_start])

/* Cuts '\0' from strings */
#define make_array_slice_string(_name_, ...) \
    static_assert( is_same_type( array_first_ref((__VA_ARGS__)), char*, 1, 1) == true, "Not a char array"); \
    make_array_slice_back(_name_, 1, (__VA_ARGS__))

#define array_slice_string(...) array_slice_back(1, (__VA_ARGS__))

/* string_literal(_name_, string_literal)
 * @_name_: variable name for new array pointer
 * @_string_: string literal, e.g "Some String"
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
    fwrite(*str, 1, P_ARRAY_SIZE(str) - 1, stdout);

    //Make slice without '\0' and print it as array of chars
    make_array_slice_string(str_nonull, str);
    print_array(str_nonull);

 * Also, you can declare string literals at global scope with static keyword (or without)

    static string_literal(useful_string, "I am just a string");

    int main() {
        println(*useful_string, " with size:", P_ARRAY_SIZE(useful_string));
        return 0;
    }

 * compilers optimizes string literals even at -O0 optimization, but do not rely on it.

    string_literal(str1, "Text ");
    string_literal(str2, "Text ");
    string_literal(str3, "Text2");

    println("str1 equals str2? ", (bool)(str1 == str2)); //true
    println("str1 equals str3? ", (bool)(str1 == str3)); //false

 */
#define string_literal(_name_, _string_) \
    declare_string_literal(_name_, _string_) = (const typeof( (_string_)[0] ) (*)[])( (_string_) )

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


/* Just prints array.
 * @__VA_ARGS__: array or pointer to an array
 * example:

    print_array(&(int[]){1, 2, 3, 4, 5, 6}); //will print: 123456

    bool sexy[] = {true, false, true, true};
    print_array(&sexy); //will print: truefalsetruetrue

    print_array(&"String is an array too!"); //will print that string. less effective than println(), but works =)
 */
#define print_array(...) do {                                \
    foreach_array_ref((__VA_ARGS__), ref) {                  \
        print(*ref);                                         \
    }                                                        \
} while (0)


/**** Obsolete ****/

#define xARRAY_SIZE(arr)       _Generic( &(arr), typeof((arr)[0]) (*)[]: sizeof((arr))/sizeof( (arr)[0] ) )
#define xARRAY_SIZE_BYTES(arr) _Generic( &(arr), typeof((arr)[0]) (*)[]: sizeof((arr)) )
#define xARRAY_ELEMENT_SIZE(arr) _Generic( &(arr), typeof((arr)[0]) (*)[]: sizeof((arr)[0]) )

#define xP_ARRAY_SIZE(arr)       _Generic( (arr), typeof((*(arr))[0]) (*)[]: sizeof(*(arr))/sizeof( (*(arr))[0] ) )
#define xP_ARRAY_SIZE_BYTES(arr) _Generic( (arr), typeof((*(arr))[0]) (*)[]: sizeof(*(arr)) )
#define xP_ARRAY_ELEMENT_SIZE(arr) _Generic( (arr), typeof((*(arr))[0]) (*)[]: sizeof((*(arr))[0]) )

#define xP_ARRAYS_SIZE(...) ( MAP(p_array_size_helper__, __VA_ARGS__) 0 )
#define p_array_size_helper__(_arr_ptr_) P_ARRAY_SIZE(_arr_ptr_) +

#define xP_ARRAYS_SIZE_BYTES(...) ( MAP(p_array_size_bytes_helper__, __VA_ARGS__) 0 )
#define p_array_size_bytes_helper__(_arr_ptr_) P_ARRAY_SIZE_BYTES(_arr_ptr_) +

/* Backward compatibility */

#define P_ARRAY_SIZE(arr) ARRAY_SIZE(arr)
#define P_ARRAY_SIZE_BYTES(arr) ARRAY_SIZE_BYTES(arr)
#define P_ARRAY_ELEMENT_SIZE(arr) ARRAY_ELEMENT_SIZE(arr)
#define P_ARRAY_ELEMENT_TYPE(arr) ARRAY_ELEMENT_TYPE(arr)

#define P_ARRAYS_SIZE(...) ARRAYS_SIZE((__VA_ARGS__))
#define P_ARRAYS_SIZE_BYTES(...) ARRAYS_SIZE_BYTES((__VA_ARGS__))



#endif // MISC_ARRAY_H
