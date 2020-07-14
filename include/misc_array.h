#ifndef MISC_ARRAY_H
#define MISC_ARRAY_H

#include <misc.h>
#include <traits.h>
#include <inttypes.h>

/* Some colors */
#define CRED    "\033[0;31m"
#define CGREEN  "\033[0;32m"
#define CYELLOW "\033[0;33m"
#define CBLUE   "\033[0;34m"
#define CMAGNETA "\033[0;35m"
#define CCYAN   "\033[0;36m"
#define CRESET  "\033[0m"

/* Define ARRAY_RUNTIME_CHECKS to activate runtime array checks, use only for debugging! */
//#define ARRAY_RUNTIME_CHECKS

#define arr_errmsg(...) ((void)printerrln(__VA_ARGS__), (void)abort(), 0)

union _dummy_type_ {char a;};
union _not_an_array_type_ {char a;};

/* returns dummy array if var is VLA. returns dummy pointer if var is pointer to VLA otherwise returns var */
#define vla_ptr_check(var)                                           \
                    if_vla(var,                                      \
                        (union _dummy_type_ [1]){0},                 \
                        if_vla(*(var),                               \
                                (union _not_an_array_type_ ***){0},  \
                                var)                                 \
                        )


/* evaluates t, if var is array or (f) if it is not. additionally checks if var is not pointer to VLA */
#define if_arr__vla_guard(var, t, f)  	 	 	 	\
        _Generic( &(typeof( vla_ptr_check(var) )){0},           \
                typeof(* vla_ptr_check(var) )(*)[]: (t),        \
                default: (f)                                    \
        )

/* This macro returns (var) if (var) is an array or returns (*var) if var is a pointer to something */
#define deref_or_arr__vla(var)  if_arr__vla_guard(var, var, *var)

/* replaces var with dummy array if var is VLA */
#define vla_check(var) if_vla(var, (union _dummy_type_ [1]){0}, var )

/* evaluates t, if var is array, do not use directly */
#define if_arr__no_vla_ptr(var, t)	 	 	\
        _Generic( & vla_check(var),                     \
                typeof(* vla_check(var) )(*)[]: (t)     \
        )

/* auto_arr(var)
 * @var: array or pointer to an array
 *
 * This macro returns (var) if (var) is an array, or (*var) if (var) is a pointer to an array
 * Basically, this macro automatically dereferences pointers to arrays.
 * Stops compilation if var is not an array or a pointer to array.
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
#define auto_arr(var)  if_arr__no_vla_ptr(deref_or_arr__vla(var), deref_or_arr__vla(var))

/* Macro alias for auto_arr() */
#if !defined(arr)
#define arr(...) auto_arr((__VA_ARGS__))
#endif

/* Returns number of elements in the array */
#define ARRAY_SIZE(arr) ( sizeof( auto_arr(arr)) / sizeof (auto_arr(arr)[0]) )
/* Returns total array size in bytes */
#define ARRAY_SIZE_BYTES(arr) sizeof( auto_arr(arr) )
/* Returns size of one element in the array */
#define ARRAY_ELEMENT_SIZE(arr) sizeof( auto_arr(arr)[0] )
/* Extracts type of array element from array */
#define ARRAY_ELEMENT_TYPE(arr) typeof( auto_arr(arr)[0] )

/* ONLY FOR INTERNAL USE! arr is not validated for arrayness. arr should be an array. */
#define UNSAFE_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) )
#define UNSAFE_ARRAY_SIZE_BYTES(arr) (sizeof(arr))
#define UNSAFE_ARRAY_ELEMENT_SIZE(arr) sizeof( (arr)[0] )
#define UNSAFE_ARRAY_ELEMENT_TYPE(arr) typeof( (arr)[0] )

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
#define PRINT_ARRAY_INFO(...) println(h_print_array_info((__VA_ARGS__), " \"" #__VA_ARGS__ "\" at "),      \
                                     ((const void*)(__VA_ARGS__)),                                         \
                                     " has size:", ARRAY_SIZE((__VA_ARGS__)),                              \
                                     " uses ", ARRAY_SIZE_BYTES((__VA_ARGS__)), " bytes,"                  \
                                     " while single element has size:", ARRAY_ELEMENT_SIZE((__VA_ARGS__)))

#define h_print_array_info(arr, append) if_vla_or_vla_ptr(arr, "VLA" append, "Array" append)
#define if_vla_or_vla_ptr(_arr_, t, f) if_constexpr(sizeof(_arr_) + sizeof(*(_arr_)), f, t)


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
#define malloc_array(_arr_ptr_) (( _arr_ptr_ = malloc( ARRAY_SIZE_BYTES(_arr_ptr_) ) ))

/* Same as malloc_array, but with calloc */
#define calloc_array(_arr_ptr_) (( _arr_ptr_ = calloc( 1, ARRAY_SIZE_BYTES(_arr_ptr_) ) ))

/* Fills array with specified byte by using memset
 * WARNING: It is better and safer to use fill_array() instead!
 * example:

    char (*data)[5];
    malloc_array(data);
    memset_array(data, 'L');

    foreach_array_ref(data, ref)
        print(*ref); //Will print: LLLLL

*/
#define memset_array(_arr_ptr_, symbol) memset((_arr_ptr_), symbol, ARRAY_SIZE_BYTES(_arr_ptr_))

/* Fills array with specified value
 * example:

    int (*ty)[5];
    malloc_array(ty);

    fill_array(ty, 50);
    print_array(ty); //Will print 5050505050

    free(ty);

 */
#define fill_array(_arr_, ...)  \
    foreach_array_ref(_arr_, _ref_) \
        *(_ref_) = (__VA_ARGS__)

/* Unsafe variant of fill_array(),
 * Should be used only with pta, arguments are not checked for arrayness */
#define unsafe_fill_array(_pta_, ...) \
    unsafe_foreach_array_ref(_pta_, _ref_) \
        *(_ref_) = (__VA_ARGS__)


/* This needs more work, don't use */
#define for_each_bit_in_array(_array_) \
for(unsigned byte_index = 0; byte_index < P_ARRAY_SIZE(_array_); byte_index++) \
    for(unsigned bit_index = 0; bit_index < (P_ARRAY_ELEMENT_SIZE(_array_) * 8); bit_index++)

/* Iterate over an array.
 * @_arr_ptr_: array or pta
 * @_ref_ptr_name_: name of pointer which will be used as reference to each array element
 * example:
 *
    uint16_t test[] = {1, 2, 3, 4};
    foreach_array_ref(&test, val)
        print(*val);  //prints: 1234

 *
 * NOTE: if _arr_ptr_ contains const data, then _ref_ptr_name_ also will be const
 */
#define foreach_array_ref(_arr_, _ref_ptr_name_) foreach_array_ref_base(,(_arr_), _ref_ptr_name_)

/* same as foreach_array_ref(), but _ref_ptr_name_ is always const */
#define foreach_array_const_ref(_arr_, _ref_ptr_name_) foreach_array_ref_base(const, (_arr_), _ref_ptr_name_)

#define foreach_array_ref_base(prefix, _arr_, _ref_ptr_name_)                           \
        for(prefix ARRAY_ELEMENT_TYPE(_arr_)                                            \
                (*const _tmp_arr_ptr_)[ARRAY_SIZE(_arr_)] = & auto_arr(_arr_),          \
                *_ref_ptr_name_ = &(*_tmp_arr_ptr_)[0];                                 \
                                                                                        \
                _ref_ptr_name_ < unsafe_array_end_ref(_tmp_arr_ptr_);                   \
                                                                                        \
                (_ref_ptr_name_)++)

/* unsafe_foreach_array_ref(), unsafe_foreach_array_const_ref():
 *   unsafe iteration over pta. pta is not checked for being pointer to array. not safe for compound literals. only for internal usage.
 * @_array_ptr: pta
 * @_ref_ptr_name_: name of new pointer for iteration
 */
#define unsafe_foreach_array_ref(_array_ptr_, _ref_ptr_name_) \
    for(unsafe_make_array_first_ref(_array_ptr_, _ref_ptr_name_); _ref_ptr_name_ != &(*(_array_ptr_))[UNSAFE_ARRAY_SIZE(*(_array_ptr_))]; _ref_ptr_name_++)

#define unsafe_foreach_array_const_ref(_array_ptr_, _ref_ptr_name_) \
    for(const unsafe_make_array_first_ref(_array_ptr_, _ref_ptr_name_); _ref_ptr_name_ != &(*(_array_ptr_))[UNSAFE_ARRAY_SIZE(*(_array_ptr_))]; _ref_ptr_name_++)


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

#define foreach_array_ref_bw_base(prefix, _arr_, _ref_ptr_name_)                            \
        for(prefix ARRAY_ELEMENT_TYPE(_arr_)                                                \
                (*const _tmp_arr_ptr_)[ARRAY_SIZE(_arr_)] = & auto_arr(_arr_),              \
                *_ref_ptr_name_ = &(*_tmp_arr_ptr_)[UNSAFE_ARRAY_SIZE(*_tmp_arr_ptr_) - 1]; \
                                                                                            \
                _ref_ptr_name_ >= &(*_tmp_arr_ptr_)[0];                                     \
                                                                                            \
                (_ref_ptr_name_)--)

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
#define make_array_first_ref(_array_ptr_, _ref_ptr_name_) ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = array_first_ref(_array_ptr_)

/* unsafe_make_array_first_ref(): creates a pointer to first array element. pta is not checked for arrayness. only for internal usage.
 * @_array_ptr_: pta
 * @_ref_ptr_name: name of new pointer */
#define unsafe_make_array_first_ref(_array_ptr_, _ref_ptr_name_) UNSAFE_ARRAY_ELEMENT_TYPE(*(_array_ptr_)) *(_ref_ptr_name_) = &(*(_array_ptr_))[0]

/* Declares pointer to last array element */
#define make_array_last_ref(_array_ptr_, _ref_ptr_name_) ARRAY_ELEMENT_TYPE(_array_ptr_) *(_ref_ptr_name_) = array_last_ref(_array_ptr_)

/* get pointer to the first, to the last and to the one past the last element of the array  */
#define array_first_ref(_array_ptr_) (& auto_arr((_array_ptr_))[0])
#define array_last_ref(_array_ptr_)  (& auto_arr((_array_ptr_))[ARRAY_SIZE(_array_ptr_) - 1])
#define array_end_ref(_array_ptr_)   (& auto_arr((_array_ptr_))[ARRAY_SIZE(_array_ptr_)])

/* Unsafe variants, _array_ptr_ should be a pta */
#define unsafe_array_first_ref(_array_ptr_) (&(*(_array_ptr_))[0])
#define unsafe_array_last_ref(_array_ptr_)  (&(*(_array_ptr_))[UNSAFE_ARRAY_SIZE(*(_array_ptr_)) - 1])
#define unsafe_array_end_ref(_array_ptr_)   (&(*(_array_ptr_))[UNSAFE_ARRAY_SIZE(*(_array_ptr_))])

/* returns true if ref points to the first, to the last or the one past the last elemenet of ther array */
#define is_first_array_ref(_arr_ptr_, _ref_) ((_ref_) == array_first_ref((_arr_ptr_)))
#define is_last_array_ref(_arr_ptr_, _ref_)  ((_ref_) == array_last_ref((_arr_ptr_)))
#define is_end_array_ref(_arr_ptr_, _ref_)   ((_ref_) == array_end_ref((_arr_ptr_)))
#define is_first_or_last_array_ref(_arr_ptr_, _ref_) ( is_first_array_ref((_arr_ptr_), (_ref_)) || is_last_array_ref((_arr_ptr_), (_ref_))  )

/* Unsafe variants, _array_ptr_ should be a pta */
#define unsafe_is_first_array_ref(_arr_ptr_, _ref_) ((_ref_) == unsafe_array_first_ref((_arr_ptr_)))
#define unsafe_is_last_array_ref(_arr_ptr_, _ref_)  ((_ref_) == unsafe_array_last_ref((_arr_ptr_)))
#define unsafe_is_end_array_ref(_arr_ptr_, _ref_)   ((_ref_) == unsafe_array_end_ref((_arr_ptr_)))

/* Copies data from src array or pta to dst array or pta per element
 * if dst array is larger or equal than source array, then entire source array will be copied to dst.
 * excess bytes in dst won't be touched.
 *
 * if dst array is smaller than src array, then only part of source will be copied from src with size equals dst array.
 *
 * dst and src arrays can be of different type. i.e. it is valid to copy data from array of ints to array of longs. But compiler may complain if losing precision.
 *
 * @dst_ptr: destination array or pta
 * @__VA_ARGS__: source array or pta
 *
 * examples:

    //Destination is larger than source:

    int dst[5] = {1,2,3,4,5};
    int src[3] = {9,8,7};
    copy_array(dst, src);
    print_array(dst); //prints: 98745
...
    //Source and Destination have different, but compatible types
    //Source is larger than Destination

    long dst[]  = {1,2};
    short src[] = {9,8,7};
    copy_array(dst, src);
    print_array(dst); //prints: 98
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
    fill_array(values, (const struct toto){0});
    copy_array(values, (const struct toto[]){
                   {.val = 1,  .string = "First"},
                   {.val = 10, .string = "Second"},
                   {.val = 30, .string = "Third"},
               });

    foreach_array_const_ref(values, ref)
            println(ref->val, " ", ref->string);

 */
#define copy_array(dst_arr, ...) do {                                    \
        make_arrview_full(_tmp_dst_ptr_, dst_arr);                       \
        const make_arrview_full(_tmp_src_ptr_, (__VA_ARGS__));           \
                                                                         \
        if(unsafe_is_ptas_of_same_types(_tmp_dst_ptr_, _tmp_src_ptr_)) { \
            unsafe_copy_array_fast(_tmp_dst_ptr_, _tmp_src_ptr_);        \
        } else {                                                         \
            unsafe_copy_array_slow(_tmp_dst_ptr_, _tmp_src_ptr_);        \
        }                                                                \
} while(0)

/* unsafe_copy_array_fast(): copies data from src_ptr pta to dst_ptr pta. types of pta are not checked for arrayness.
 * Use only if you guarantee that dst_ptr and src_ptr are ptas and have same underlying type.
 * @dst_ptr: destination pta
 * @src_ptr: source pta to copy from */
#define unsafe_copy_array_fast(_dst_ptr_, _src_ptr_)                                    \
    memcpy(_dst_ptr_, _src_ptr_,                                                        \
           UNSAFE_ARRAY_SIZE_BYTES(*_dst_ptr_) >= UNSAFE_ARRAY_SIZE_BYTES(*_src_ptr_) ? \
            UNSAFE_ARRAY_SIZE_BYTES(*_src_ptr_) :                                       \
            UNSAFE_ARRAY_SIZE_BYTES(*_dst_ptr_)                                         \
    )

/* unsafe_copy_array_slow(): copies data from src_ptr pta to dst_ptr pta. types of pta are not checked for arrayness.
 * Use only if you guarantee that dst_ptr and src_ptr are ptas and have compatible underlying type.
 * @dst_ptr: destination pta
 * @src_ptr: source pta to copy from */
#define unsafe_copy_array_slow(_dst_ptr_, _src_ptr_) do {                    \
    if(UNSAFE_ARRAY_SIZE((*_dst_ptr_)) >= UNSAFE_ARRAY_SIZE((*_src_ptr_))) { \
        unsafe_make_array_first_ref(_dst_ptr_, _dst_ref_);                   \
        unsafe_foreach_array_const_ref(_src_ptr_, _src_ref_) {               \
            *_dst_ref_++ = *_src_ref_;                                       \
        }                                                                    \
    } else {                                                                 \
        const unsafe_make_array_first_ref(_src_ptr_, _src_ref_);             \
        unsafe_foreach_array_ref(_dst_ptr_, _dst_ref_) {                     \
            *_dst_ref_ = *_src_ref_++;                                       \
        }                                                                    \
    }                                                                        \
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


/* Copies multiple arrays into single array
 * @dst_ptr: pointer to destination array
 * __VA_ARGS__: pointers to source arrays to copy data from
 *
 * All source arrays must contain same element type as destination array
 * Destination array size must be equal or larger than total size of all source arrays.
 *
 * example:

    make_arrview_str(src1, "This ");
    make_arrview_str(src2, "Is ");
    make_arrview_str(src3, "A string");

    char dst[ARRAYS_SIZE(src1, src2, src3)];
    copy_arrays(dst, src1, src2, src3);

    print_array(dst); //prints: This Is A string

 */
#define copy_arrays(_dst_arr_, ...) do {                                        \
    make_array_first_ref(_dst_arr_, _tmp_ptr_);                                 \
    (                                                                           \
        MAP_LIST_TWOARG(helper_copy_arrays, _tmp_ptr_, _dst_arr_, __VA_ARGS__)  \
    );                                                                          \
} while(0)

#define helper_copy_arrays(_tmp_ptr_, _dst_array_, _src_array_) \
    static_assert_expr(is_arrays_of_same_types( _dst_array_ , _src_array_), "Source array: " #_src_array_ " doesn't have same type as destination array"), \
    (void)(_tmp_ptr_ = ((typeof(_tmp_ptr_))memcpy(_tmp_ptr_, _src_array_, ARRAY_SIZE_BYTES(_src_array_))) + ARRAY_SIZE(_src_array_))

/***** Arrview *****/


/* make_arrview(name, start, size, src): make an array view of the array src with size (size) starting from (start) position.
 * example:

  uint8_t data[] = {0,1,2,3,4,5};
  make_arrview(data_slc, 2, 3, &data); //Start index: 2 and size: 3
  print_array(data_slc); //prints: 234
*/
#define make_arrview(_name_, start, size, ...) ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ (size) ] = arrview(start, size, __VA_ARGS__)
#define arrview(start, size, ...) h_arrview_(start, size, (__VA_ARGS__))

/* (make_)array_slice_size() runtime and static error checking */
#if defined (ARRAY_RUNTIME_CHECKS)
#define h_arrview_(start, size, _arr_) h_arrview__dyncheck(start, size, _arr_)
#else
#define h_arrview_(start, size, _arr_) h_arrview__static(start, size, (_arr_))
#endif

#define h_arrview__static(start, size, _arr_) \
    (ARRAY_ELEMENT_TYPE(_arr_) (*) [  h_arrview__check_size_static(_arr_, start, size)  ]) &(auto_arr(_arr_) [start])

#define h_arrview__check_size_static(_arr_, start, size) _Generic(1,                    \
                int*:   sizeof(char [start < 0 ? -1 : 1]),                        \
                int**:  sizeof(char [size  < 1 ? -1 : 1]),                        \
                int***: sizeof(char [start + size > ARRAY_SIZE(_arr_) ? -1 : 1]), \
                default: size )

#define h_arrview__dyncheck(start, size, _arr_) \
    ((void)h_arrview__check_size_dyn(_arr_, start, size), (ARRAY_ELEMENT_TYPE(_arr_) (*) [size]) &(auto_arr(_arr_) [start]))

#define h_arrview__check_size_dyn(_arr_, start, size) (                                                                                                     \
        (void)(sizeof(char [start < 0 ? -1 : 1]) != 1 ?                                                                                               \
        arr_errmsg(CRED "Start index less than 0 when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),                           \
        (void)(sizeof(char [size < 1 ? -1 : 1]) != 1 ? 													                                              \
        arr_errmsg(CRED "Size is less than 1 when creating arrview slice at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),                         \
        (void)(sizeof(char [start + size > ARRAY_SIZE(_arr_) ? -1 : 1]) != 1 ?                                                                        \
        arr_errmsg(CRED "Sum of start position and size is larger than source array size when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__) \
        " start:", start, " size:", size, " array size:", ARRAY_SIZE(_arr_), CRESET) : 0)                                                             \
    )

/* make_arrview_first(name, size, src): create arrview of the first (size) elements of the array src */
#define make_arrview_first(_name_, size, ...) make_arrview(_name_, 0, size, __VA_ARGS__)
#define arrview_first(size, ...) arrview(0, size, __VA_ARGS__)

/* make_arrview_last(name, size, src): create arrview of the last (size) elements of the array src
 * example:

  uint8_t data[] = {0,1,2,3,4,5};
  make_arrview_last(data_slc, 2, &data);
  print_array(data_slc); //prints: 45
 */
#define make_arrview_last(_name_, size, ...) ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ (size) ] = arrview_last(size, __VA_ARGS__)
#define arrview_last(size, ...) array_slice_last_(size, (__VA_ARGS__))

/* (make_)array_slice_last() error check */
#if defined (ARRAY_RUNTIME_CHECKS)
#define array_slice_last_(size, src_arr) arrslc_last_dyncheck(size, src_arr)
#else
#define array_slice_last_(size, src_arr) arrslc_last_static(size, src_arr)
#endif

#define arrslc_last_static(size, src_arr) \
    (ARRAY_ELEMENT_TYPE(src_arr) (*) [ chk_arrslc_last_static(src_arr, size) ]) (array_end_ref(src_arr) - size)

#define chk_arrslc_last_static(arr, size) _Generic(1,                           \
                int*:  sizeof(char [size > ARRAY_SIZE(arr) ? -1 : 1]),          \
                int**: sizeof(char [size < 1 ? -1 : 1]),                        \
                default: size )

#define arrslc_last_dyncheck(size, src_arr) \
    ((void)chk_arrslc_last_dyn(src_arr, size), (ARRAY_ELEMENT_TYPE(src_arr) (*) [size])(array_end_ref(src_arr) - size) )

#define chk_arrslc_last_dyn(arr, size) (                                                                                                \
        (void)(sizeof(char [size < 1 ? -1 : 1]) != 1 ? 													                                \
        arr_errmsg(CRED "Size is less than 1 when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),             \
        (void)(sizeof(char [size > ARRAY_SIZE(arr) ? -1 : 1]) != 1 ?                                                                    \
        arr_errmsg(CRED "Size is larger than source array size when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__)               \
        " size:", size, " array size:", ARRAY_SIZE(arr), CRESET) : 0)                                                                   \
    )

/* make_arrview_shrink(name, skip_start, skip_end, src): Make an array view by skipping (skip_start) elements from the beginning and (skip_end) elements from the end of the array src
 * example:

  char data[] = {0, 0, 'H','I', 0, 0, 0};
  make_arrview_shrink(data_slc, 2, 3, &data);
  print_array(data_slc); //prints: HI
*/
#define make_arrview_shrink(_name_, skip_start, skip_end, ...) \
    ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ ARRAY_SIZE((__VA_ARGS__)) - (skip_start) - (skip_end) ] = arrview_shrink(skip_start, skip_end, __VA_ARGS__)

#define arrview_shrink(skip_start, skip_end, ...) array_slice_shrink_(skip_start, skip_end, (__VA_ARGS__))

/* (make_)array_slice_shrink() runtime and static error checking */
#if defined (ARRAY_RUNTIME_CHECKS)
#define array_slice_shrink_(skip_start, skip_end, ...) arrslc_shrink_dyncheck(skip_start, skip_end, __VA_ARGS__)
#else
#define array_slice_shrink_(skip_start, skip_end, ...) arrslc_shrink_static(skip_start, skip_end, __VA_ARGS__)
#endif

#define arrslc_shrink_static(skip_start, skip_end, ...) \
        (ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*) [ chk_arrslc_shrink_static((__VA_ARGS__), (skip_start), (skip_end)) ]) &(auto_arr((__VA_ARGS__))[skip_start])

#define chk_arrslc_shrink_static(arr, skip_start, skip_end) _Generic(1,                 \
    int*:   sizeof(char [skip_start < 0 ? -1 : 1]),                                     \
    int**:  sizeof(char [skip_end < 0 ? -1 : 1]),                                       \
    int***: sizeof(char [skip_start >= ARRAY_SIZE(arr) ? -1 : 1 ] ),                    \
    char*:  sizeof(char [skip_end >= ARRAY_SIZE(arr) ? -1 : 1 ] ),                      \
    char**: sizeof(char [skip_start + skip_end >= ARRAY_SIZE(arr) ? -1 : 1 ] ),         \
    default: ARRAY_SIZE(arr) - (skip_start) - (skip_end)                                \
    )

#define arrslc_shrink_dyncheck(skip_start, skip_end, ...) \
         ((void)chk_arrslc_shrink_dyn((__VA_ARGS__), skip_start, skip_end) ,(ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*) [ ARRAY_SIZE((__VA_ARGS__)) - (skip_start) - (skip_end) ]) &(auto_arr((__VA_ARGS__))[skip_start]))

#define chk_arrslc_shrink_dyn(arr, skip_start, skip_end) (                                                                                                      \
        (void)(sizeof(char [skip_start < 0 ? -1 : 1]) != 1 ?                                                                                                    \
            arr_errmsg(CRED "Start index less than 0 when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),                                       \
        (void)(sizeof(char [skip_end < 0 ? -1 : 1]) != 1 ?                                                                                                      \
            arr_errmsg(CRED "End index is less than 0 when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),                                \
        (void)(sizeof(char [skip_start >= ARRAY_SIZE(arr) ? -1 : 1]) != 1 ? 											\
            arr_errmsg(CRED "Start index is equals or greater than array size when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__),	                \
            " skip_start:", skip_start, " array size:", ARRAY_SIZE(arr), CRESET) : 0),                                                                              \
        (void)(sizeof(char [skip_end >= ARRAY_SIZE(arr) ? -1 : 1]) != 1 ?                                                                                       \
            arr_errmsg(CRED "End index is equals or greater than array size when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__),                	\
            " skip_end:", skip_end, " array size:", ARRAY_SIZE(arr), CRESET) : 0),                                                                                  \
        (void)(sizeof(char [skip_start + skip_end >= ARRAY_SIZE(arr) ? -1 : 1]) != 1 ?                                                                          \
            arr_errmsg(CRED "Sum of start position and end position is equals or greater than source array size when creating arrview at " __FILE__ ":" STRINGIFY2(__LINE__) \
            " skip_start:", skip_start, " skip_end:", skip_end, " array size:", ARRAY_SIZE(arr), CRESET) : 0)                                                       \
    )


/* make_arrview_cfront(name, start, src): make arrview by skipping (skip_start) elements from the beginning of the array src */
#define make_arrview_cfront(_name_, skip_start, ...) make_arrview_shrink(_name_, skip_start, 0, __VA_ARGS__)
#define arrview_cfront(skip_start, ...) arrview_shrink(skip_start, 0, __VA_ARGS__)

/* make_arrview_cback(name, skip_end, src): make arrview by skipping (skip_end) elements from the end of the array src */
#define make_arrview_cback(_name_, skip_end, ...) make_arrview_shrink(_name_, 0, skip_end, __VA_ARGS__)
#define arrview_cback(skip_end, ...) arrview_shrink(0, skip_end, __VA_ARGS__)

/* make_arrview_full(name, src): make full arrview of the array src */
#define make_arrview_full(_name_, ...) ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ARRAY_SIZE((__VA_ARGS__))] = arrview_full((__VA_ARGS__))
#define arrview_full(...) &(auto_arr((__VA_ARGS__)))

/* make_arrview_str(name, src): makr arrview without last element, useful for cutting '\0' from C strings */
#define make_arrview_str(_name_, ...) make_arrview_cback(_name_, 1, __VA_ARGS__)
#define arrview_str(...) arrview_cback(1, __VA_ARGS__)s

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


/*** Beta features ****/

#define TAKE_FIRST_ARG(var, ...) var

/* Typeof with stripped qualifiers */
#define TYPEOF_NO_QUAL(var) typeof( (typeof(var) (*)(void) ){0}() )

/* Returns type of array element without qualifiers, works only on single-dimensional arrays */
#define ARRAY_ELEMENT_TYPE_NO_QUAL(var) TYPEOF_NO_QUAL(ARRAY_ELEMENT_TYPE(var))

/* make_merged_array(_name_, arr1, arr2, ..., arrn)
 * @_name_: name of new array to create
 * @__VA_ARGS__: source arrays to copy data from
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

    print_array(data); //prints: 01234567891011

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
#define array_remove_view(_arr_ptr_, _view_)                                               \
    memmove(_view_, array_end_ref(_view_),                                                 \
    (array_last_ref(_arr_ptr_) - array_last_ref(_view_)) * ARRAY_ELEMENT_SIZE(_arr_ptr_))  \

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
    make_v_view, 2, 3, v); //pointer to elements '2', '3', '4'
    array_remove_view_fill(v, v_view, 9);
    print_array(v); //prints: 015678999

 */
#define array_remove_view_fill(_arr_ptr_, _view_, _val_)  do {     \
    array_remove_view(_arr_ptr_, _view_);                          \
    make_arrview_last(_to_replace_, ARRAY_SIZE(_view_), _arr_ptr_);\
    unsafe_fill_array(_to_replace_, _val_);                        \
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
    print_array(v); //prints: 01344

 */
#define array_remove_ref(_arr_ptr_, _ref_)                                       \
    memmove((_ref_),                                                             \
            (_ref_) + 1,                                                         \
            (array_last_ref(_arr_ptr_) - (_ref_)) * sizeof(*(_ref_)))            \

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
    print_array(v); //prints: 01349

 */
#define array_remove_ref_fill(_arr_ptr_, _ref_, _val_) \
    ((void) array_remove_ref(_arr_ptr_, _ref_),        \
     (void)(*array_last_ref(_arr_ptr_) = (_val_)))


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
#define array_insert(_arr_, _ref_, _val_)                                   \
    (                                                                       \
    (void)memmove((_ref_) + 1,                                              \
                 (_ref_),                                                   \
                 (array_last_ref(_arr_) - (_ref_)) * sizeof(*(_ref_))),     \
    (void)(*(_ref_) = (_val_))                                              \
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
    print_array(v);

 */
#define array_insert_array(_arr_, _idx_, ...) h_array_insert_array(_arr_, _idx_, (__VA_ARGS__))

#define h_array_insert_array(_arr_, _idx_, _src_arr_) do {                                                             \
    make_arrview(_to_replace_, _idx_, ARRAY_SIZE(_src_arr_), _arr_);                                                   \
    memmove(unsafe_array_end_ref(_to_replace_),                                                                        \
            unsafe_array_first_ref(_to_replace_),                                                                      \
            (array_last_ref(_arr_) - unsafe_array_last_ref(_to_replace_)) * UNSAFE_ARRAY_ELEMENT_SIZE(*_to_replace_)); \
    copy_array(_to_replace_, _src_arr_);                                                                               \
} while (0)

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

/* Prints array as [x,y,z,...] */
#define print_array_fmt(...) do {                                      \
    const make_arrview_full(_tmp_arr_ptr_, __VA_ARGS__);               \
    print("[", *unsafe_array_first_ref(_tmp_arr_ptr_));                \
                                                                       \
    if(UNSAFE_ARRAY_SIZE(*_tmp_arr_ptr_) > 1) {                        \
        make_arrview_cfront(_tmp_arr_ptr2_, 1, _tmp_arr_ptr_);         \
        unsafe_foreach_array_ref(_tmp_arr_ptr2_, ref)                  \
            print(",", *ref);                                          \
    }                                                                  \
    println("]");                                                      \
} while (0)

/* Array bit operations */

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

#define print_array_bits(arr) do {                                   \
    print("|");                                                      \
    foreach_array_bit_bw(arr, bit_idx) {                             \
        print( array_get_bit(arr, bit_idx) ? CGREEN "X" : CRED "O"); \
                                                                     \
        if(!(bit_idx % (ARRAY_ELEMENT_SIZE(arr) * 8)))               \
            print(CRESET "|");                                       \
        else if( !(bit_idx % 8) )                                    \
            print(CBLUE "|");                                        \
    }                                                                \
    println(CRESET "\n");                                            \
} while(0)

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
#define array_ref_remove(_arr_ptr_, _ref_, _val_)  array_remove_ref_fill(_arr_ptr_, _ref_, _val)

#endif // MISC_ARRAY_H
