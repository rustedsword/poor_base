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

/***** Experimental *****/

/* Number of maximum dereferences into multi-dimensional arrays */
#define MAX_ARRAY_DEREF 5

typedef union {char a;} more_than_maximum_supported_array_dimensions_reached;

/* if size of var is known at compile time, just returns var, otherwise returns _dummy_type_ */
#define drop_vla(var) if_constexpr(sizeof(var), var, (union _dummy_type_){0})

/* evaluates (t) if var is array, otherwise evaluates (f)
 * @var: variable to test for array type
 *
 * WARNING: This macro will produce false positive on anonymous structs and unions declared inside compound literals
 * like that:
 *      if_arr_unsafe((struct {char a;}){.a = 1} , true, false) //returns true
 *      if_arr_unsafe((struct {char a;}***){0} , true, false) //returns true
 *      if_arr_unsafe((union {char a;}***){0} , true, false) //returns true
 * also, it doesn't work with anonymous enums
 *      if_arr_unsafe((enum {Pp, ppp}){0} , true, false) //fails compilation due to redeclaration of enumerator
 *
 * This macro compares var with typeof(var).
 * for all of C types that i know, this selection will match, except arrays.
 * If var is an array it will decay in _Generic controlling expression to pointer to it's first element,
 * while any other types won't change
 *
 * Additionally, drop_vla macro will return _dummy_type_, if var is VLA, which forces default selector
 */
//#define if_arr_unsafe(var, t, f) _Generic(&(var), typeof(NULL, drop_vla(var))*:(f), default:(t))
//#define if_arr_unsafe(var, t, f) _Generic(&(var), typeof(NULL, drop_vla(var))*:(f), default:(t))
#ifdef __clang__
#define if_arr_unsafe(var, t, f) _Generic(&(var), \
        typeof(NULL, drop_vla(var))* :(f),\
        typeof(NULL, drop_vla(var))const* :(f),\
        typeof(NULL, drop_vla(var))const volatile*:(f),\
        typeof(NULL, drop_vla(var))const _Atomic volatile*:(f),\
        typeof(NULL, drop_vla(var))const _Atomic *:(f),\
        typeof(NULL, drop_vla(var))_Atomic volatile*:(f),\
        typeof(NULL, drop_vla(var))_Atomic*:(f),\
         default:(t))
#else
//#define if_arr_unsafe(var, t, f) _Generic(&(var),
//        typeof(NULL, drop_vla(var))* :(f),
//         default:(t))
#define if_arr_unsafe(var, t, f) _Generic((NULL, var), \
        typeof(drop_vla(var)) :(f),\
         default:(t))
#endif


/* this macro is same as if_arr_unsafe(), but it is using statement expression extension
 * to avoid issues of _unsafe() variant. it is correctly evaluates (t) and (f) so far,
 * but, since we rely on default _Generic selector, it may break in future C standards, if more types become 'incompatible' with itself
 */
#define if_arr(var, t, f) ({ typeof(var) tmp__; if_arr_unsafe(tmp__, t, f);  })

/* evaluates (t), if var is array, or stops compilation
 * This macro explicitly compares var with pointer to array type
 * it is not replacement for if_arr() macro, because var is derefenced here,
 * so any non-pointer and any non-array type will result in a sytax error, making this macro useful only for stopping compilation.
 *
 * also, it won't work on arrays of anonymous structs declared inside of compound literals
 *      if_arr_strict((struct {char a;}[1]){0} , println("Shit")); //fails compilation, while shouldn't
 *
 * NOTE: Doesn't work on vla
 */
#define if_arr_strict_unsafe(var, t) _Generic(&(var), typeof(*(var)) (*)[] : (t))

/* Same as _unsafe variant. always works, but uses statement expression extension */
#define if_arr_strict(var, t) ({ typeof(var) tmp__; if_arr_strict_unsafe(tmp__, (t)); })

#define deref_array_(var)  (* if_arr((var), var, &var) ) /* if var is array then returns value at index 0 or returns var */

/*Prevents further dereferencing, by failing compilation if MAX_ARRAY_DEREF reached */
#define deref_array_guard_(var) (* if_arr((var), (more_than_maximum_supported_array_dimensions_reached){0}, &var) )

#define deref_array_1(var) deref_array_(var)
#define deref_array_2(var) deref_array_(deref_array_(var))
#define deref_array_3(var) deref_array_(deref_array_(deref_array_(var)))
#define deref_array_4(var) deref_array_(deref_array_(deref_array_(deref_array_(var))))
#define deref_array_5(var) deref_array_(deref_array_(deref_array_(deref_array_(deref_array_(var)))))

/* Dereferences multi-dimensional array and returns element at index 0 from last dimension
 * maximum dereference depth is MAX_ARRAY_DEREF
 * char a[2][3][4] = {0};
 * int b[10][3] = {0};
 *
 * //These lines are the same:
 * char val = ARRAY_DEREF(a);
 *      val = a[0][0][0];
 *
 * int v = ARRAY_DEREF(b);
 *     v = b[0][0];
 *
 * typeof(ARRAY_DEREF(a)) tmp = 1; //tmp is char
 *
 * due to exponential macro growth, this macro works only on arrays, pointers to arrays should be dereferenced manually
 */
#define ARRAY_DEREF(var) if_arr((var), deref_array_guard_( CAT(deref_array_, MAX_ARRAY_DEREF)(var)  ), (union _not_an_array_type_){0})

typedef union { char a; } alvl;
/* if var is not array creates alvl[] array with size equals dimension where first non-array vaule detected */
#define get_arr_dim1(var) (* if_arr((var), (var), &(alvl [1]){0} ) )

/* checks, if var is array. if it is array, dereferences it,
 * ignoring arrays with alvl type, otherwise creates alvl array with size equals dim_lvl */
#define get_arr_dim_(var, dim_lvl) (* if_arr((var), _Generic(&(var), alvl(*)[]: &(var), default: (var) ), &(alvl [dim_lvl]){0} ) )

#define arr_dim_sel1_(var) get_arr_dim1(var)
#define arr_dim_sel2_(var) get_arr_dim_(var, 2)
#define arr_dim_sel3_(var) get_arr_dim_(var, 3)
#define arr_dim_sel4_(var) get_arr_dim_(var, 4)
#define arr_dim_sel5_(var) get_arr_dim_(var, 5)

#define arr_dim_get1(var) (& (arr_dim_sel1_( deref_array_(var) )))
#define arr_dim_get2(var) (& (arr_dim_sel2_(arr_dim_sel1_( deref_array_(var) ))))
#define arr_dim_get3(var) (& (arr_dim_sel3_(arr_dim_sel2_(arr_dim_sel1_( deref_array_(var) )))))
#define arr_dim_get4(var) (& (arr_dim_sel4_(arr_dim_sel3_(arr_dim_sel2_(arr_dim_sel1_( deref_array_(var) ))))))
#define arr_dim_get5(var) (& (arr_dim_sel5_(arr_dim_sel4_(arr_dim_sel3_(arr_dim_sel2_(arr_dim_sel1_( deref_array_(var) )))))))

#define get_arr_dim_n(var) \
        _Generic( CAT(arr_dim_get, MAX_ARRAY_DEREF)(var), \
                alvl(*)[1]: 1, \
                alvl(*)[2]: 2, \
                alvl(*)[3]: 3, \
                alvl(*)[4]: 4, \
                alvl(*)[5]: 5, \
        default: (more_than_maximum_supported_array_dimensions_reached){0})

/* Returns number of array dimensions up to MAX_ARRAY_DEREF,
 * @var: array
 * due to exponential macro growth this macro works only on arrays.
 * i.e pointers to arrays are not supported, you have to dereference them manually. */
#define ARRAY_DIMENSIONS(var) if_arr((var), get_arr_dim_n(var), (union _not_an_array_type_){0})

/* checks if var and dereferenced var is VLA. returns _dummy_type_ in this case to prevent passing real array or pointer inside typeof */
#define drop_vla_p(var) if_constexpr(sizeof(var) + sizeof(*var), var, (union _dummy_type_){0})

/* var can be a pointer to array, or array itself, as usual we must not let VLA type pass to typeof()
 * however, pointer to vla cannot be specified inside _Generic selection. so we must check twice for VLA here ... */
#define if_arr_unsafe_p(var, t, f)                              \
        _Generic((NULL, var),                                         \
        typeof(drop_vla_p(var)):(f),                            \
        default:                                                \
                if_constexpr(sizeof(var),                       \
                         if_constexpr(sizeof(*var), (t), (f) )  \
                ,(t) )                                          \
        )

/* Dereferences var if it is not an array */
#define deref_if_not_array(var) if_arr_unsafe_p((var), (var), *(var))
/* Checks if var is array or pointer to array */
#define ____auto_arr(var) if_arr_unsafe(deref_if_not_array(var), deref_if_not_array(var), (union _not_an_array_type_){0})

/***** end experimental *****/

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
#define ___print_array_helper(arr, append) (is_vla( auto_arr(arr)) ? "VLA" append : "Array" append)
#define PRINT_ARRAY_INFO(...) println(___print_array_helper(__VA_ARGS__, " \"" #__VA_ARGS__ "\" at "), ((const void*)(__VA_ARGS__)) ,	\
                                     " has size:", ARRAY_SIZE((__VA_ARGS__)),                                                                         \
                                     " uses ", ARRAY_SIZE_BYTES((__VA_ARGS__)), " bytes,"                                                             \
                                     " while single element has size:", ARRAY_ELEMENT_SIZE((__VA_ARGS__)))


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
                _ref_ptr_name_ != &(*_tmp_arr_ptr_)[UNSAFE_ARRAY_SIZE(*_tmp_arr_ptr_)]; \
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

/* get pointer to first array element */
#define array_first_ref(_array_ptr_) (& auto_arr((_array_ptr_))[0])

/* get pointer to last array element */
#define array_last_ref(_array_ptr_) (& auto_arr((_array_ptr_))[ ARRAY_SIZE(_array_ptr_) - 1 ])

/* get pointer to the array element after the last one. While this is a valid pointer, IT SHOULD NOT BE DEREFERENCED! */
#define array_end_ref(_array_ptr_) (& auto_arr((_array_ptr_))[ ARRAY_SIZE(_array_ptr_) ])

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

  uint8_t a[] = {1, 2, 3, 4, 5};
  uint8_t *ref = &a[2]; //pointer to value '3'
  array_remove_ref(&a, ref, 50);
  //  array now will be: {1, 2, 4, 5, 50}

  ref = &(a[4]); //pointer to last element: '50'
  array_remove_ref(&a, ref, 0);
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

    int dst[5] = {1,2,3,4,5};
    int src[3] = {9,8,7};
    copy_array(dst, src);
    print_array(dst); //prints: 98745
...
    int dst[] = {1,2};
    int src[] = {9,8,7};
    copy_array(dst, src);
    print_array(dst); //prints: 98
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
#define copy_array(dst_arr, ...) do {                   \
        make_array_slice_full(dst, dst_arr);            \
        make_array_slice_full(src, (__VA_ARGS__));      \
        if(unsafe_is_ptas_of_same_types(dst, src)) {    \
            unsafe_copy_array_fast(dst, src);           \
        } else {                                        \
            unsafe_copy_array_slow(dst, src);           \
        }                                               \
} while(0)

/* unsafe_copy_array_fast(): copies data from src_ptr pta to dst_ptr pta. types of pta are not checked for arrayness.
 * Use only if you guarantee that dst_ptr and src_ptr are ptas and have same underlying type.
 * @dst_ptr: destination pta
 * @src_ptr: source pta to copy from */
#define unsafe_copy_array_fast(dst_ptr, src_ptr)                                \
    (UNSAFE_ARRAY_SIZE_BYTES(*dst_ptr) >= UNSAFE_ARRAY_SIZE_BYTES(*src_ptr))    \
        ? memcpy(dst_ptr, src_ptr, UNSAFE_ARRAY_SIZE_BYTES(*src_ptr))           \
        : memcpy(dst_ptr, src_ptr, UNSAFE_ARRAY_SIZE_BYTES(*dst_ptr))           \

/* unsafe_copy_array_slow(): copies data from src_ptr pta to dst_ptr pta. types of pta are not checked for arrayness.
 * Use only if you guarantee that dst_ptr and src_ptr are ptas and have compatible underlying type.
 * @dst_ptr: destination pta
 * @src_ptr: source pta to copy from */
#define unsafe_copy_array_slow(dst_ptr, src_ptr) do {                   \
    if(UNSAFE_ARRAY_SIZE(*dst_ptr) >= UNSAFE_ARRAY_SIZE((*src_ptr))) {  \
        unsafe_make_array_first_ref(dst_ptr, dst_ref);                  \
        unsafe_foreach_array_const_ref(src_ptr, src_ref) {              \
            *dst_ref++ = *src_ref;                                      \
        }                                                               \
    } else {                                                            \
        const make_array_first_ref(src_ptr, src_ref);                   \
        foreach_array_ref(dst_ptr, dst_ref) {                           \
            *dst_ref = *src_ref++;                                      \
        }                                                               \
    }                                                                   \
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

    make_array_slice_string(src1, "This ");
    make_array_slice_string(src2, "Is ");
    make_array_slice_string(src3, "A string");
    char dst[ARRAYS_SIZE(src1, src2, src3)];
    copy_arrays(dst, src1, src2, src3);
    print_array(dst); //prints: This Is A string

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
   ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_ ) [ ARRAY_SIZE((__VA_ARGS__)) - (start) ] = array_slice_front(start, (__VA_ARGS__))

#define array_slice_front(start, ...) (ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*)[ ARRAY_SIZE((__VA_ARGS__)) - start ]) &(auto_arr((__VA_ARGS__))[start])


/* make array slice, skipping (end) elements at array end
 * example:

    //Cut '\0' from string and print it as array
    make_array_slice_back(string_view, 1, &"String is an array too!");
    print_array(string_view);

 */
#define make_array_slice_back(_name_, end, ...) \
   ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_ ) [ ARRAY_SIZE((__VA_ARGS__)) - (end) ] = array_slice_back(end, (__VA_ARGS__))

#define array_slice_back(end, ...) (ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*)[ ARRAY_SIZE((__VA_ARGS__)) - end ]) &(auto_arr((__VA_ARGS__))[0])


/* make array slice with size (size), skipping (start) elements at front
 * example:

    uint8_t data[] = {0,1,2,3,4,5};
    make_array_slice_size(data_slc, 2, 3, &data); //Start index: 2 and size: 3
    print_array(data_slc); //prints: 234
*/
#define make_array_slice_size(_name_, start, size, ...) ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ (size) ] = array_slice_size(start, size, __VA_ARGS__)

#define array_slice_size(start, size, ...) array_slice_size_(start, size, __VA_ARGS__)

/* (make_)array_slice_size() runtime and static error checking */
#if defined (ARRAY_RUNTIME_CHECKS)
#define array_slice_size_(start, size, ...) arrslc_size_dyncheck(start, size, __VA_ARGS__)
#else
#define array_slice_size_(start, size, ...) arrslc_size_static(start, size, __VA_ARGS__)
#endif

#define arrslc_size_static(start, size, ...) \
    (ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*) [  chk_arrslc_size_static((__VA_ARGS__), start, size)  ]) &(auto_arr((__VA_ARGS__)) [start])

#define chk_arrslc_size_static(arr, start, size) _Generic(1,                       \
                int*:   sizeof(char [start + size > ARRAY_SIZE(arr) ? -1 : 1]),    \
                int**:  sizeof(char [start < 0 ? -1 : 1]),                         \
                int***: sizeof(char [size  < 1 ? -1 : 1]),                         \
                default: size )

#define arrslc_size_dyncheck(start, size, ...) \
    ((void)chk_arrslc_size_dyn((__VA_ARGS__), start, size), (ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (*) [size]) &(auto_arr((__VA_ARGS__)) [start]))

#define chk_arrslc_size_dyn(arr, start, size) (                                                                                         \
        (void)(sizeof(char [start < 0 ? -1 : 1]) != 1 ?													                                \
        arr_errmsg(CRED "Start index less than 0 when creating slice at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),			    \
        (void)(sizeof(char [size < 1 ? -1 : 1]) != 1 ? 													                                \
        arr_errmsg(CRED "Size is less than 1 when creating array slice at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),			    \
        (void)(sizeof(char [start + size > ARRAY_SIZE(arr) ? -1 : 1]) != 1 ?										                    \
        arr_errmsg(CRED "Sum of start position and size is larger than source array size when creating slice at " __FILE__ ":" STRINGIFY2(__LINE__) \
        " start:", start, " size:", size, " array size:", ARRAY_SIZE(arr), CRESET) : 0)					                	            \
    )

/* Make array slice while skipping n bytes from start and n bytes from end
 * example:

   char data[] = {0, 0, 'H','I', 0, 0, 0};
   make_array_slice_shrink(data_slc, 2, 3, &data);
   print_array(data_slc); //prints: HI

 */
#define make_array_slice_shrink(_name_, skip_start, skip_end, ...) \
        ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ ARRAY_SIZE((__VA_ARGS__)) - (skip_start) - (skip_end) ] = array_slice_shrink(skip_start, skip_end, (__VA_ARGS__))

#define array_slice_shrink(skip_start, skip_end, ...) array_slice_shrink_(skip_start, skip_end, __VA_ARGS__)

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
            arr_errmsg(CRED "Start index less than 0 when creating slice at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),                                       \
        (void)(sizeof(char [skip_end < 0 ? -1 : 1]) != 1 ?                                                                                                      \
            arr_errmsg(CRED "End index is less than 0 when creating array slice at " __FILE__ ":" STRINGIFY2(__LINE__) CRESET) : 0),                                \
        (void)(sizeof(char [skip_start >= ARRAY_SIZE(arr) ? -1 : 1]) != 1 ? 											\
            arr_errmsg(CRED "Start index is equals or greater than array size when creating array slice at " __FILE__ ":" STRINGIFY2(__LINE__),	                \
            " skip_start:", skip_start, " array size:", ARRAY_SIZE(arr), CRESET) : 0),                                                                              \
        (void)(sizeof(char [skip_end >= ARRAY_SIZE(arr) ? -1 : 1]) != 1 ?                                                                                       \
            arr_errmsg(CRED "End index is equals or greater than array size when creating array slice at " __FILE__ ":" STRINGIFY2(__LINE__),                	\
            " skip_end:", skip_end, " array size:", ARRAY_SIZE(arr), CRESET) : 0),                                                                                  \
        (void)(sizeof(char [skip_start + skip_end >= ARRAY_SIZE(arr) ? -1 : 1]) != 1 ?                                                                          \
            arr_errmsg(CRED "Sum of start position and end position is equals or greater than source array size when creating slice at " __FILE__ ":" STRINGIFY2(__LINE__) \
            " skip_start:", skip_start, " skip_end:", skip_end, " array size:", ARRAY_SIZE(arr), CRESET) : 0)                                                       \
    )


/* make full slice
 * size of array slice will be equal original array
 * basically, this is just a copy of pointer to source array */
#define make_array_slice_full(_name_, ...) \
        ARRAY_ELEMENT_TYPE((__VA_ARGS__)) (* _name_) [ARRAY_SIZE((__VA_ARGS__))] = array_slice_full((__VA_ARGS__))

#define array_slice_full(...) &(auto_arr((__VA_ARGS__)))

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
