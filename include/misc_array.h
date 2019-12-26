#ifndef MISC_ARRAY_H
#define MISC_ARRAY_H

#include <misc.h>

union __you_should_proivde_array_or_pointer_to_array_here___note__array_of_structs_are_not_supported { int a;};
#define _gen_array_specifiers(_array_, _type_) \
        _type_ (*)[]: (_array_), \
        _type_ (**)[]: *(_array_),

/* This macro used for easy access to arrays
 * if you send array here, it will just return same array
 * if you send pointer to array here, it will return dereferenced pointer to that array
 *
 * int a[20];
 * arr(a)[5] = 40; same as: a[5] = 40;
 *
 * int (*ptr)[20] = &a;
 * arr(ptr)[1] = 10; -> same as: (*ptr)[1] = 10;
 */
#define arr(_array_) _Generic(&(_array_), \
        MAP_ARG(_gen_array_specifiers, _array_, \
                char, signed char, unsigned char, \
                signed short, unsigned short, \
                signed int, unsigned int, \
                signed long, unsigned long, \
                signed long long, unsigned long long, \
                float, double, long double, \
                char *, const char*, \
                void *, const void*, \
                bool) \
        default: (union __you_should_proivde_array_or_pointer_to_array_here___note__array_of_structs_are_not_supported*)(_array_))

/* Returns number of elements in the array, _array_ can be array or pointer to array.
 * this only works for arrays of basic C types */
#define XARRAY_SIZE(_array_) ARRAY_SIZE( arr(_array_) )
#define XARRAY_SIZE_BYTES(_array_) ARRAY_SIZE_BYTES( arr(_array_) )
#define XARRAY_SIZE_ELEMENT(_array_) ARRAY_ELEMENT_SIZE( arr(_array_) )

/* Returns number of elements in the array, if arr is not an array then compilation will fail */
#define ARRAY_SIZE(arr)       _Generic( &(arr), typeof((arr)[0]) (*)[]: sizeof((arr))/sizeof( (arr)[0] ) )
/* Returns whole array size in bytes */
#define ARRAY_SIZE_BYTES(arr) _Generic( &(arr), typeof((arr)[0]) (*)[]: sizeof((arr)) )
/* Returns size of one element in the array */
#define ARRAY_ELEMENT_SIZE(arr) _Generic( &(arr), typeof((arr)[0]) (*)[]: sizeof((arr)[0]) )

/* Returns number of elements in the array. arr should be pointer to an array or compilation will fail */
#define P_ARRAY_SIZE(arr)       _Generic( (arr), typeof((*(arr))[0]) (*)[]: sizeof(*(arr))/sizeof( (*(arr))[0] ) )
/* Returns array's size in bytes, arr should be pointer to array */
#define P_ARRAY_SIZE_BYTES(arr) _Generic( (arr), typeof((*(arr))[0]) (*)[]: sizeof(*(arr)) )
/* Returns size of one element in the array, arr should be pointer to array */
#define P_ARRAY_ELEMENT_SIZE(arr) _Generic( (arr), typeof((*(arr))[0]) (*)[]: sizeof((*(arr))[0]) )

/* Get sum of array sizes
 * example:

    char te[] = {1, 2, 3};
    long me[] = {5, 6};
    println(ARRAYS_SIZE(te, me)); //prints: 5
*/
#define ARRAYS_SIZE(...) ( MAP(array_size_helper__, __VA_ARGS__) 0 )
#define array_size_helper__(arr) ARRAY_SIZE(arr) +

/* same as ARRAYS_SIZE() but for pointers to arrays */

#define P_ARRAYS_SIZE(...) ( MAP(p_array_size_helper__, __VA_ARGS__) 0 )
#define p_array_size_helper__(_arr_ptr_) P_ARRAY_SIZE(_arr_ptr_) +

/* Get total size in bytes for all arrays
 * example:

    uint8_t te[] = {1, 2, 3};
    uint16_t me[] = {5, 6};
    println(ARRAYS_SIZE_BYTES(te, me)); //prints: 7
 */
#define ARRAYS_SIZE_BYTES(...) ( MAP(array_size_bytes_helper__, __VA_ARGS__) 0 )
#define array_size_bytes_helper__(arr) ARRAY_SIZE_BYTES(arr) +

/* same as ARRAYS_SIZE_BYTES() but for pointers to arrays */
#define P_ARRAYS_SIZE_BYTES(...) ( MAP(p_array_size_bytes_helper__, __VA_ARGS__) 0 )
#define p_array_size_bytes_helper__(_arr_ptr_) P_ARRAY_SIZE_BYTES(_arr_ptr_) +

/* extracts type of array element from pointer to array */
#define P_ARRAY_ELEMENT_TYPE(_array_ptr_) typeof((*(_array_ptr_))[0])

/* Prints information about array
 * @__VA_ARGS__: pointer to an array
 * example:

    PRINT_ARRAY_INFO(&(long long[]){1, 2, 3, 4});
    //prints: Array "&(long long[]){1, 2, 3, 4}" at (0x7fffffffdc40) has size:4 uses 32 bytes, while single element has size:8

 */
#define PRINT_ARRAY_INFO(...) println("Array \"" #__VA_ARGS__ "\" at (", ((const void*)(__VA_ARGS__)) ,")" \
                                     " has size:", P_ARRAY_SIZE((__VA_ARGS__)), \
                                     " uses ", P_ARRAY_SIZE_BYTES((__VA_ARGS__)), \
                                     " bytes, while single element has size:", P_ARRAY_ELEMENT_SIZE((__VA_ARGS__)))


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
for(unsigned byte_index = 0; byte_index < XARRAY_SIZE(_array_); byte_index++) \
    for(unsigned bit_index = 0; bit_index < (XARRAY_SIZE_ELEMENT(_array_) * 8); bit_index++)

/* Iterate over an array.
 * @_arr_ptr_: pointer to array
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
    for(array_first_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ != (*(_arr_ptr_)) + P_ARRAY_SIZE((_arr_ptr_)); (_ref_ptr_name_)++  )

/* same as foreach_array_ref(), but _ref_ptr_name_ is always const */
#define foreach_array_const_ref(_arr_ptr_, _ref_ptr_name_) \
     for(const array_first_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ != (*(_arr_ptr_)) + P_ARRAY_SIZE((_arr_ptr_)); (_ref_ptr_name_)++  )

/*
 * Iterate backwards over an array. Same as foreach_array_ref(), but backwards.
 * example:
 *
    uint16_t test[] = {1, 2, 3, 4};
    foreach_array_ref_bw(&test, val)
        print(*val);  //prints: 4321
 */
#define foreach_array_ref_bw(_arr_ptr_, _ref_ptr_name_) \
    for(array_last_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ >= (*(_arr_ptr_)); (_ref_ptr_name_)--  )

/* same as foreach_array_ref_bw(), but _ref_ptr_name_ is always const */
#define foreach_array_const_ref_bw(_arr_ptr_, _ref_ptr_name_) \
    for(const array_last_ref(_arr_ptr_, _ref_ptr_name_); _ref_ptr_name_ >= (*(_arr_ptr_)); (_ref_ptr_name_)--  )


/* Get index of an array element where _ref_ is pointing.
 * example:

    uint64_t test[] = {0x2000, 100, 200, 300};
    foreach_array_ref(test, val) {
        println("Array index:", array_ref_index(test, val), " Array value:0x", to_hex(*val, 8));

        if(array_ref_index(test, val) == 2)
            println("Magic index 2 detected, value at index 1:", *(val-1) , " and value and index 3:", test[3]);
    }

*/
#define array_ref_index(_array_, _ref_) ((_ref_) - &((_array_))[0])

/* Declares pointer to first array element */
#define array_first_ref(_array_ptr_, _ref_ptr_name_) P_ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = get_array_first_ref(_array_ptr_)

/* Declares pointer to last array element */
#define array_last_ref(_array_ptr_, _ref_ptr_name_) P_ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = get_array_last_ref(_array_ptr_)

/* get pointer to first array element */
#define get_array_first_ref(_array_ptr_) (&(*(_array_ptr_))[0])

/* get pointer to last array element */
#define get_array_last_ref(_array_ptr_) (&(*(_array_ptr_))[ P_ARRAY_SIZE(_array_ptr_) - 1 ])

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
        const typeof(__VA_ARGS__) src_ptr = (__VA_ARGS__);          \
        if(is_arrays_of_same_types(dst_ptr, src_ptr)) {             \
            copy_array_fast__(dst_ptr, src_ptr);                      \
        } else {                                                    \
            copy_array_slow__(dst_ptr, src_ptr);                      \
        }                                                           \
} while(0)

#define is_arrays_of_same_types(dst_ptr, src_ptr)     \
        _Generic((dst_ptr),                        \
            typeof((*(src_ptr))[0]) (*)[]: true,   \
            default: false                         \
        )

#define copy_array_fast__(dst_ptr, src_ptr)                           \
    (P_ARRAY_SIZE_BYTES(dst_ptr) >= P_ARRAY_SIZE_BYTES(src_ptr))    \
        ? memcpy(dst_ptr, src_ptr, P_ARRAY_SIZE_BYTES(src_ptr))     \
        : memcpy(dst_ptr, src_ptr, P_ARRAY_SIZE_BYTES(dst_ptr))     \

#define copy_array_slow__(dst_ptr, src_ptr) do {                  \
    if(P_ARRAY_SIZE(dst_ptr) >= P_ARRAY_SIZE((src_ptr))) {      \
        array_first_ref(dst_ptr, dst_ref);                     \
        foreach_array_const_ref(src_ptr, src_ref) {          \
            *dst_ref++ = *src_ref;                              \
        }                                                       \
    } else {                                                    \
        const array_first_ref(src_ptr, src_ref);                \
        foreach_array_ref(dst_ptr, dst_ref) {                  \
            *dst_ref = *src_ref++;                              \
        }                                                       \
    }                                                           \
} while(0)


/* makes array slice
 * @_name_: name of variable to declare for slice
 * @_start_: start index
 * @_end_: end index
 * @__VA_ARGS__: pointer to an array from which array slice will be made
 *
 * @start should not be less or equal @end
 * both @start and @end should not be equal or greater than source array size
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
    make_array_slice(slice, 6, ARRAY_SIZE(vla), &vla);
    print_array(slice); //prints: float:5.000000
...
    //Copy three string into one

    // Pack string literals into constant pointers to arrays of constant chars
    make_array_slice_literal(pre_string, "Everyone ");
    make_array_slice_literal(mid_string, "Must ");
    make_array_slice_literal(post_string, "Die");

    // Strip '\0' from first two strings
    make_array_slice_string(pre, pre_string);
    make_array_slice_string(mid, mid_string);

    // Create array
    char (*string)[P_ARRAYS_SIZE(pre, mid, post_string)];
    malloc_array(string);

    //Make slice from string with size of array pre
    make_array_slice_size(string_slice0, 0, P_ARRAY_SIZE(pre), string);
    copy_array(string_slice0, pre);

    //Make slice from string with size of array mid and with start position after pre string
    make_array_slice_size(string_slice1, P_ARRAY_SIZE(pre), P_ARRAY_SIZE(mid), string);
    copy_array(string_slice1, mid);

    //Make slice from string at position after pre and mid.
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
   typeof( (*(__VA_ARGS__))[0] ) (*(_name_)) [ (end) - (start) ] = (typeof( (*(__VA_ARGS__))[0] ) (*)[])( (*(__VA_ARGS__)) + (start) )

/* make array slice, skipping (start) elements at front */
#define make_array_slice_front(_name_, start, ...) \
   { __attribute__((unused)) char start_index_is_greater_than_array_size_minus_one [ (P_ARRAY_SIZE((__VA_ARGS__)) > start ) ? 1 : -1];} \
   typeof( (*(__VA_ARGS__))[0] ) (*(_name_)) [ P_ARRAY_SIZE((__VA_ARGS__)) - (start) ] = (typeof( (*(__VA_ARGS__))[0] ) (*)[])( (*(__VA_ARGS__)) + (start) )

/* make array slice, skipping (end) elements at array end
 * example:

    //Cut '\0' from string and print it as array
    make_array_slice_back(string_view, 1, &"String is an array too!");
    print_array(string_view);

 */
#define make_array_slice_back(_name_, end, ...) \
   { __attribute__((unused)) char end_index_is_greater_than_array_size_minus_one [ (P_ARRAY_SIZE((__VA_ARGS__)) > end ) ? 1 : -1];} \
   typeof( (*(__VA_ARGS__))[0] ) (*(_name_)) [ P_ARRAY_SIZE((__VA_ARGS__)) - (end) ] = (typeof( (*(__VA_ARGS__))[0] ) (*)[])( (*(__VA_ARGS__)) )

/* make array slice with size (size), skipping (start) elements at front */
#define make_array_slice_size(_name_, start, size, ...) \
   { __attribute__((unused)) char start_and_size_together_greater_than_array_size[ (P_ARRAY_SIZE((__VA_ARGS__)) >= start + size ) ? 1 : -1];} \
   { __attribute__((unused)) char size_should_not_be_zero[ size ? 1 : -1 ];  }                                       \
   typeof( (*(__VA_ARGS__))[0] ) (*(_name_)) [ (size) ] =  (typeof( (*(__VA_ARGS__))[0] ) (*)[]) ( (*(__VA_ARGS__)) + (start) )

/* Cuts '\0' from strings */
#define make_array_slice_string(_name_, ...) \
    _Static_assert( is_same_type( get_array_first_ref((__VA_ARGS__)), char*, 1, 1) == true, "Not a char array"); \
    make_array_slice_back(_name_, 1, (__VA_ARGS__))

/* string_literal(_name_, string_literal)
 * @_name_: variable name for new array pointer
 * @_string_: string literal, e.g "Some String"
 *
 * Special macro for string literals.
 * Declares constant pointer to array of constant chars and sets it's value to provided string literal
 *
 * example:

    string_literal(str, "String Literal");
    //str = NULL; // <--Error
    //str[0] = '1'; // <--Error

    //Dereference to const char* and print
    println(*str);
    printf("%s\n", *str);
    fwrite(*str, 1, P_ARRAY_SIZE(str), stdout);

    //Make slice without '\0' and print it as array of chars
    make_array_slice_string(str_nonull, str);
    print_array(str_nonull);

 * Also, you can declare array slice literals at global scope with static keyword (or without)

    static string_literal(useful_string, "I am just a string");

    int main() {
        println(*useful_string);
        return 0;
    }

 */
#define string_literal(_name_, _string_) \
    declare_string_literal_pointer(_name_, _string_) = (const typeof( (_string_)[0] ) (*)[])( (_string_) )

/* Only declares constant pointer to array of constant chars
 * can be used with extern.
 * example:
 *
    //Export string literal from library

//  library.h:
    #define SUPER_STRING "This is string!"
    extern declare_string_literal_pointer(super_string, SUPER_STRING);

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
#define declare_string_literal_pointer(_name_, _string_) \
    const typeof( (_string_)[0] ) (*const (_name_)) [ is_same_type( get_array_first_ref(&(_string_)), char*, 1, 0) ? ARRAY_SIZE(_string_) : -1 ]

//const typeof( (_string_)[0] ) (*const (_name_)) [ is_same_type( get_array_first_ref(&(_string_)), char*, 1, 0) ? ARRAY_SIZE(_string_) : -1 ] = (const typeof( (_string_)[0] ) (*)[])( (_string_) )


/* Just prints array.
 * @__VA_ARGS__: pointer to an array
 * example:

    print_array(&(int[]){1, 2, 3, 4, 5, 6}); //will print: 123456

    bool sexy[] = {true, false, true, true};
    print_array(&sexy); //will print: truefalsetruetrue

    print_array(&"String is an array too!"); //will print that string. less effective than println(), but works =)
 */
#define print_array(...) do {                                \
    const typeof(__VA_ARGS__) array_ptr = (__VA_ARGS__);     \
    foreach_array_ref(array_ptr, ref)                       \
        print(*ref);                                         \
    } while (0)

#endif // MISC_ARRAY_H
