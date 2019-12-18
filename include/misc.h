#ifndef MISC_H
#define MISC_H

#include "map.h"
#include "stdio.h"
#include "stdbool.h"
#include <string.h>
#include <time.h>

//#define CAT(A, B) CAT2(A, B)
//#define CAT2(A, B) A ## B

/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    cloak.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define BITAND(x) PRIMITIVE_CAT(BITAND_, x)
#define BITAND_0(y) 0
#define BITAND_1(y) y

#define INC(x) PRIMITIVE_CAT(INC_, x)
#define INC_0 1
#define INC_1 2
#define INC_2 3
#define INC_3 4
#define INC_4 5
#define INC_5 6
#define INC_6 7
#define INC_7 8
#define INC_8 9
#define INC_9 9

#define DEC(x) PRIMITIVE_CAT(DEC_, x)
#define DEC_0 0
#define DEC_1 0
#define DEC_2 1
#define DEC_3 2
#define DEC_4 3
#define DEC_5 4
#define DEC_6 5
#define DEC_7 6
#define DEC_8 7
#define DEC_9 8

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
#define PROBE(x) x, 1,

#define IS_PAREN(x) CHECK(IS_PAREN_PROBE x)
#define IS_PAREN_PROBE(...) PROBE(~)

#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
#define NOT_0 PROBE(~)

#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define BOOL(x) COMPL(NOT(x))

#define IIF(c) PRIMITIVE_CAT(IIF_, c)
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t

#define IF(c) IIF(BOOL(c))

#define EAT(...)
#define EXPAND(...) __VA_ARGS__
#define WHEN(c) IF(c)(EXPAND, EAT)

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(id) id DEFER(EMPTY)()

#define REPEAT(count, macro, ...) \
    WHEN(count) \
    ( \
        OBSTRUCT(REPEAT_INDIRECT) () \
        ( \
            DEC(count), macro, __VA_ARGS__ \
        ) \
        OBSTRUCT(macro) \
        ( \
            DEC(count), __VA_ARGS__ \
        ) \
    )
#define REPEAT_INDIRECT() REPEAT

#define WHILE(pred, op, ...) \
    IF(pred(__VA_ARGS__)) \
    ( \
        OBSTRUCT(WHILE_INDIRECT) () \
        ( \
            pred, op, op(__VA_ARGS__) \
        ), \
        __VA_ARGS__ \
    )
#define WHILE_INDIRECT() WHILE

#define PRIMITIVE_COMPARE(x, y) IS_PAREN \
( \
    COMPARE_ ## x ( COMPARE_ ## y) (())  \
)

#define IS_COMPARABLE(x) IS_PAREN( CAT(COMPARE_, x) (()) )

#define NOT_EQUAL(x, y) \
IIF(BITAND(IS_COMPARABLE(x))(IS_COMPARABLE(y)) ) \
( \
   PRIMITIVE_COMPARE, \
   1 EAT \
)(x, y)

#define EQUAL(x, y) COMPL(NOT_EQUAL(x, y))

#define COMMA() ,

#define COMMA_IF(n) IF(n)(COMMA, EAT)()


/*
 * Real classic of true Macro!
 */
#define container_of(ptr, type, member) ( (type *)( (uintptr_t)(ptr) - offsetof(type, member) ))

#define memcpy_and_null(dst, src, len) \
    memcpy(dst, src, len );            \
    dst[len] = '\0'

/*
 * COUNT_PARAMS will return count of parameters passed, up to 62
 */
#define PP_ARG_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

#define PP_RSEQ_N()                                        \
         63, 62, 61, 60,                                   \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define PP_NARG_(...)    PP_ARG_N(__VA_ARGS__)

#define PP_COMMASEQ_N()                                    \
          1,  1,  1,  1,                                   \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  0,  0

#define PP_COMMA(...)    ,

#define PP_HASCOMMA(...)                                   \
          PP_NARG_(__VA_ARGS__, PP_COMMASEQ_N())

#define PP_NARG(...)                                       \
          PP_NARG_HELPER1(                                 \
              PP_HASCOMMA(__VA_ARGS__),                    \
              PP_HASCOMMA(PP_COMMA __VA_ARGS__ ()),        \
              PP_NARG_(__VA_ARGS__, PP_RSEQ_N()))

#define PP_NARG_HELPER1(a, b, N)    PP_NARG_HELPER2(a, b, N)
#define PP_NARG_HELPER2(a, b, N)    PP_NARG_HELPER3_ ## a ## b(N)
#define PP_NARG_HELPER3_01(N)    0
#define PP_NARG_HELPER3_00(N)    1
#define PP_NARG_HELPER3_11(N)    N

/* Compatibility */
#define COUNT_PARMS(...)     PP_NARG(__VA_ARGS__)

/*
 * this macroses will create zero-terminated slice of string and
 * allocate it using variable length array, alloca or malloc
 *
 * @src is source string
 * @start is index of the first byte, starting from zero
 * @count is number of bytes to copy
 *
 * example:
 *  char * string = "this is just a string";
 *  string_slice_vla_unsafe(slice, string, 0, 4);
 *  printf("slice contains:%s and it's length is %zu\n", slice, sizeof(slice));
 *
 * Will print:
 * > slice contains:this and it's length is 5
 */

#define string_slice_vla(var_name, src, start, count) \
    char var_name [count + 1];                        \
    memcpy_and_null(var_name, src + start, count);

#define string_slice_alloca(src, start, count) ({ \
    char *slice = alloca(count + 1);              \
    memcpy_and_null(slice, src + start, count);   \
    slice;})

#define string_slice(src, start, count) ({      \
    char *slice = malloc(count + 1);            \
    if(!slice)                                  \
        NULL;                                   \
    memcpy_and_null(slice, src + start, count); \
    slice;})


/*
 * Safe variant of string slice.
 *
 * Arguments are same as in string_slice_vla_unsafe.
 *
 * @src must be zero terminated string
 */
#define string_slice_vla_safe(var_name, src, start, count) \
    size_t _len##var_name = strlen(src); \
    size_t _var_name##start = start > _len##var_name ? _len##var_name : start; \
    size_t _var_name##count = start + count > _len##var_name ? _len##var_name - start : count; \
    char var_name [_var_name##count + 1]; \
    memcpy(var_name, src + _var_name##start, _var_name##count); \
    var_name[_var_name##count] = '\0'


/*
 * Turns standard types into printf's format specifier
 */
#define EXTRA 0
#define printf_dec_format(x) _Generic((x), \
    char: "%c", \
    signed char: "%hhd", \
    unsigned char: "%hhu", \
    signed short: "%hd", \
    unsigned short: "%hu", \
    signed int: "%d", \
    unsigned int: "%u", \
    long int: "%ld", \
    unsigned long int: "%lu", \
    long long int: "%lld", \
    unsigned long long int: "%llu", \
    float: "%f", \
    double: "%f", \
    long double: "%Lf", \
    char *: "%s", \
    const char *: "%s", \
    void *: "%p", \
    const void *: "%p", \
    IF(EXTRA)(EXTRA_SPECIFIER, EAT)() \
    bool: "%s" \
)

static inline char _p_char(char c) { return c; }
static inline signed char _p_schar(signed char c) { return c; }
static inline unsigned char _p_uchar(unsigned char c) { return c; }

static inline signed short _p_sshort(signed short c) { return c; }
static inline unsigned short _p_ushort(unsigned short c) { return c; }

static inline signed int _p_sint(signed int c) { return c; }
static inline unsigned int _p_uint(unsigned int c) { return c; }

static inline signed long int _p_slong(signed long int c) { return c; }
static inline unsigned long int _p_ulong(unsigned long int c) { return c; }

static inline signed long long int _p_sllong(signed long long int c) { return c; }
static inline unsigned long long int _p_ullong(unsigned long long int c) { return c; }

static inline float _p_float(float c) { return c; }
static inline double _p_double(double c) { return c; }
static inline long double _p_ldouble(long double c) { return c;}

static inline char * _p_char_ptr(char *c) { return c; }
static inline const char * _p_cchar_ptr(const char *c) { return c; }

static inline void * _p_void_ptr(void *c) { return c; }
static inline const void * _p_cvoid_ptr(const void *c) { return c; }

static inline const char * _p_bool(bool c) { return c ? "true" : "false"; }

#define _each_printf_args(arg) _Generic((arg),\
        char: _p_char,                        \
        signed char: _p_schar,                \
        unsigned char: _p_uchar,              \
        signed short: _p_sshort,              \
        unsigned short: _p_ushort,            \
        signed int: _p_sint,                  \
        unsigned int: _p_uint,                \
        long int: _p_slong,                   \
        unsigned long int: _p_ulong,          \
        long long int: _p_sllong,             \
        unsigned long long int: _p_ullong,    \
        float: _p_float,                      \
        double: _p_double,                    \
        long double: _p_ldouble,              \
        char *: _p_char_ptr,                  \
        const char *: _p_cchar_ptr,           \
        void *: _p_void_ptr,                  \
        const void *: _p_cvoid_ptr,           \
        IF(EXTRA)(EXTRA_SPECIFIER_PRINT, EAT)() \
        bool: _p_bool                         \
)((arg))

/* This macro is used to convert values for printing
 * currently only bool converted to strings "true" or "false". other values are printed as is */
#define printf_args_pre_process(...) \
    MAP_LIST(_each_printf_args, __VA_ARGS__)

/*
 * printf_specifier_string(endl, ...):
 * returns static single zero-terminated string with format specifiers for all variables passed
 * @endl: if set to non zero then it will add \n symbol to printf specifiers string
 */
#define _gen_printf_specifier(x) const char CAT(arr , __COUNTER__ ) [sizeof(printf_dec_format(x)) - 1];
#define _add_endline(...) const char endl;
#define _add_endline_symbol(...) '\n',

#define printf_specifier_string(endl, ...) ({            \
struct printf_specifiers {                               \
    MAP(_gen_printf_specifier, __VA_ARGS__)              \
    IF(endl)(_add_endline, EAT)()                        \
    const char null;                                     \
};                                                       \
                                                         \
union struct_as_array {                                  \
    const struct printf_specifiers p;                    \
    const char arr [sizeof(struct printf_specifiers)];   \
};                                                       \
                                                         \
static const union struct_as_array __struct_as_array = { \
    .p = {                                               \
        MAP_LIST(printf_dec_format, __VA_ARGS__),        \
        IF(endl)(_add_endline_symbol, EAT)()             \
        0                                                \
    }                                                    \
};                                                       \
__struct_as_array.arr;                                   \
})


/*
 * Prints any number(almost) of any standard variables
 */
#define print(...)   printf(printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define println(...) printf(printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

#define fprint(stream, ...)   fprintf(stream, printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define fprintln(stream, ...) fprintf(stream, printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

#define printerr(...)   fprint(stderr, __VA_ARGS__)
#define printerrln(...) fprintln(stderr, __VA_ARGS__)

#define dprint(fd, ...)   dprintf(fd, printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define dprintln(fd, ...) dprintf(fd, printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

#define sprint(buf, ...) sprintf(buf, printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

/*
 * Concatenates any number of variables of any type into buffer
 * allocated with variable length array, alloca or malloc
 *
 * Examples:
 *
 * int num = 2; uint64_t l = 500; const char *str = "some string";
 *
 * concat_vla(str_vla, "num is:", num, " var l is:", l, " string:", str);
 * println("VLA string: ", str_vla, " it's size:", sizeof(str_vla));
 *
 * char *abuf = concat_alloca("num is:", num, " var l is:", l, " string:", str);
 * println("Stack string: ", abuf);
 *
 * char *buf = concat("num is:", num, " var l is:", l, " string:", str);
 * println("Heap string: ", buf);
 * free(buf);
 */

#define concat_vla(var_name, ...) \
    const char * const fmt___ ## var_name = printf_specifier_string(0, __VA_ARGS__); \
    char var_name[snprintf(NULL, 0, fmt___ ## var_name, __VA_ARGS__) + 1];        \
    sprintf(var_name, fmt___ ## var_name, __VA_ARGS__);                           \
    var_name[ sizeof(var_name) - 1  ] = '\0'

#define concat_alloca(...) ({ \
    const char * const fmt = printf_specifier_string(0, __VA_ARGS__); \
    size_t size = snprintf(NULL, 0, fmt, __VA_ARGS__);             \
    char * string = alloca(size + 1);                              \
    sprintf(string, fmt, __VA_ARGS__);                             \
    string[size] = '\0';                                           \
    string;                                                        \
})

#define concat(...) ({ \
    const char * const fmt = printf_specifier_string(0, __VA_ARGS__); \
    size_t size = snprintf(NULL, 0, fmt, __VA_ARGS__);             \
    char * string = malloc(size + 1);                              \
    if(!string)                                                    \
        NULL;                                                      \
    sprintf(string, fmt, __VA_ARGS__);                             \
    string[size] = '\0';                                           \
    string;                                                        \
})


/* Obsolete */

static inline void printf_bool(const char *fmt, bool val) {
    (void)fmt;
    printf(val ? "true" : "false");
}
/*
 * Prints any standard varable
 */
#define print_var(x) _Generic((x),  \
    bool: printf_bool,              \
    default: printf                 \
)(printf_dec_format(x), x)

#define print_var_nl(x) print_var(x); printf("\n")

#endif // MISC_H
