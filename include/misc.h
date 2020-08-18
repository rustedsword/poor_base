#ifndef MISC_H
#define MISC_H

#include "map.h"
#include "stdio.h"
#include "stdbool.h"
#include "assert.h"

/* assert.h on windows have no static_assert define =\ */
#if defined WIN32 && !defined static_assert && !defined __cplusplus && defined _MSC_VER
# undef static_assert
# define static_assert _Static_assert
#endif

/* Redefine true and false so they will be really bools, not ints */
#ifndef __cplusplus
#ifdef true
#undef true
#define true ((bool)+1)
#endif

#ifdef false
#undef false
#define false ((bool)+0)
#endif
#endif //__cplusplus

/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    cloak.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define STRINGIFY_(var) #var
#define STRINGIFY2(var) STRINGIFY_(var)

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
 * container_of(ptr, type, member): get pointer to a struct by using pointer to some member of that struct 
 *
 * @ptr: pointer to some member of struct
 * @type: type of struct where ptr points to
 * @member: name of struct's member
 *
 * usage:
  struct s {
	char c;
	int a;
  };

  struct s s1 = {0};
  int *a_ptr = &s1.a; // a_ptr points to member 'a' of the struct s1

  struct s *s1_ptr = container_of(a_ptr, struct s, a);

  //now s1_ptr will point to s1, so (s1_ptr == &s1) will be true

 */
#define container_of(ptr, type, member) _Generic(1, int*: (void)(&(type){0}.member - (ptr)), \
                                         default: helper_container_of(ptr, type, member) )
#define helper_container_of(ptr, type, member) (type *)( (uintptr_t)(ptr) - offsetof(type, member) )

#define memcpy_and_null(dst, src, len) \
    memcpy(dst, src, len );            \
    dst[len] = '\0'

/*
 * PP_NARG will return count of parameters passed, up to 63
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


/* Test macro helpers for single or multiple arguments in variadic argument list */
#define ARG_TST_END2(...) 0, 0
#define ARG_TST_END1(...) ARG_TST_END2
#define ARG_TST_END(...) ARG_TST_END1

#define ARG_TEST3(test, t, f, ...) f
#define ARG_TEST2(test, t, f) ARG_TEST3(test, t, f, 0)
#define ARG_TEST1(test, t, f) ARG_TEST2(ARG_TST_END test, t, f )

#define SINGLE_ARG_TEST(t, f, _1, test, ...) ARG_TEST1(test, t, f)

/* Returns 1 if single argument passed in variadic argument list, or 0 if more than one argument passed */
#define IS_SINGLE_ARG(...) SINGLE_ARG_TEST(1, 0, __VA_ARGS__, (0)(0)(0), 0)
/* Expands t if single argument passed in variadic argument list, or f if more than one argument passed */
#define IF_SINGLE_ARG(t, f, ...) SINGLE_ARG_TEST(t, f, __VA_ARGS__, (0)(0)(0), 0)

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

/*** unpack basic 1-to-1 */

static inline char               _p_char(char c)            { return c; }
static inline signed char        _p_schar(signed char c)    { return c; }
static inline unsigned char      _p_uchar(unsigned char c)  { return c; }

static inline short              _p_sshort(short c)         { return c; }
static inline unsigned short     _p_ushort(unsigned short c){ return c; }

static inline int                _p_sint(int c)             { return c; }
static inline unsigned           _p_uint(unsigned c)        { return c; }

static inline long               _p_slong(long c)           { return c; }
static inline unsigned long      _p_ulong(unsigned long c)  { return c; }

static inline signed long long   _p_sllong(signed long long c)  { return c; }
static inline unsigned long long _p_ullong(unsigned long long c){ return c; }

static inline float              _p_float(float c)          { return c; }
static inline double             _p_double(double c)        { return c; }
static inline long double        _p_ldouble(long double c)  { return c; }

static inline const char *       _p_cchar_ptr(const char *c){ return c; }
static inline const char *       _p_cchar_arr_ptr(const char (*c)[]){ return *c; }

static inline const void *       _p_cvoid_ptr(const void *c){ return c; }

static inline const char *       _p_bool(bool c) { return c ? "true" : "false"; }

/* simple hex 1-1*/

typedef union { unsigned char v; }      _hex_uchar;
typedef union { unsigned short v; }     _hex_ushort;
typedef union { unsigned v; }           _hex_uint;
typedef union { unsigned long v; }      _hex_ulong;
typedef union { unsigned long long v; } _hex_ullong;

static inline _hex_uchar  _to_hex_uchar(unsigned char c)       { return (_hex_uchar){c}; }
static inline _hex_ushort _to_hex_ushort(unsigned short c)     { return (_hex_ushort){c}; }
static inline _hex_uint   _to_hex_uint(unsigned c)             { return (_hex_uint){c}; }
static inline _hex_ulong  _to_hex_ulong(unsigned long c)       { return (_hex_ulong){c}; }
static inline _hex_ullong _to_hex_ullong(unsigned long long c) { return (_hex_ullong){c}; }

static inline unsigned char       _p_hex_uchar(_hex_uchar c){ return c.v; }
static inline unsigned short      _p_hex_ushort(_hex_ushort c){ return c.v; }
static inline unsigned            _p_hex_uint(_hex_uint c){ return c.v; }
static inline unsigned long       _p_hex_ulong(_hex_ulong c){ return c.v; }
static inline unsigned long long  _p_hex_ullong(_hex_ullong c){ return c.v; }

/* Wrap number to unsigned hex type: println(fmt_hex(1)); -> printf("%x\n", 1);  */
#define fmt_hex(var) _Generic((var),                    \
    signed char:        _to_hex_uchar ,                 \
    unsigned char:      _to_hex_uchar ,                 \
    short:              _to_hex_ushort,                 \
    unsigned short:     _to_hex_ushort,                 \
    int:                _to_hex_uint  ,                 \
    unsigned:           _to_hex_uint  ,                 \
    long:               _to_hex_ulong ,                 \
    unsigned long:      _to_hex_ulong ,                 \
    long long:          _to_hex_ullong,                 \
    unsigned long long: _to_hex_ullong                  \
)(var)

/* printf modifiers */
typedef union { int p; } format_precision;
typedef union { int p; } format_width;
typedef union { int p; } format_width_zero;
typedef union { int p; } format_and_precision;
typedef union { int p; } format_hash;

static inline format_precision     _pack_precision(int p)     { return (format_precision){p}; }
static inline format_width         _pack_width(int p)         { return (format_width){p}; }
static inline format_width_zero    _pack_width_zero(int p)    { return (format_width_zero){p}; }
static inline format_and_precision _pack_and_precision(int p) { return (format_and_precision){p}; }

static inline int _unpack_precision(format_precision p) { return p.p; }
static inline int _unpack_width(format_width p) { return p.p; }
static inline int _unpack_width_zero(format_width_zero p) { return p.p; }
static inline int _unpack_and_precision(format_and_precision p) { return p.p; }

/* Basic raw types to use with format modifiers */

typedef union {signed char v;}          schar_raw;
typedef union {unsigned char v;}        uchar_raw;

typedef union {short v;}                sshort_raw;
typedef union {unsigned short v;}       ushort_raw;

typedef union {int v;}                  sint_raw;
typedef union {unsigned v;}             uint_raw;

typedef union {long v;}                 slong_raw;
typedef union {unsigned long v;}        ulong_raw;

typedef union {long long v;}            sllong_raw;
typedef union {unsigned long long v;}   ullong_raw;

typedef union {float v;}                float_raw;
typedef union {double v;}               double_raw;
typedef union {long double v;}          ldouble_raw;

typedef union { const char *v; }        const_char_ptr_raw;

/* hex types to use with format modifiers */

typedef union { unsigned char v; }      _hex_uchar_raw;
typedef union { unsigned short v; }     _hex_ushort_raw;
typedef union { unsigned v; }           _hex_uint_raw;
typedef union { unsigned long v; }      _hex_ulong_raw;
typedef union { unsigned long long v; } _hex_ullong_raw;

/* pack functions */

static inline schar_raw   _to_psn_schar(signed char c)        { return (schar_raw){c}; }
static inline uchar_raw   _to_psn_uchar(unsigned char c)      { return (uchar_raw){c}; }

static inline sshort_raw  _to_psn_sshort(short c)             { return (sshort_raw){c}; }
static inline ushort_raw  _to_psn_ushort(unsigned short c)    { return (ushort_raw){c}; }

static inline sint_raw    _to_psn_sint(int c)                 { return (sint_raw){c}; }
static inline uint_raw    _to_psn_uint(unsigned c)            { return (uint_raw){c}; }

static inline slong_raw   _to_psn_slong(long c)               { return (slong_raw){c}; }
static inline ulong_raw   _to_psn_ulong(unsigned long c)      { return (ulong_raw){c}; }

static inline sllong_raw  _to_psn_sllong(long long c)         { return (sllong_raw){c}; }
static inline ullong_raw  _to_psn_ullong(unsigned long long c){ return (ullong_raw){c}; }

static inline float_raw   _to_psn_float(float c)              { return (float_raw){c}; }
static inline double_raw  _to_psn_double(float c)             { return (double_raw){c}; }
static inline ldouble_raw _to_psn_ldouble(float c)            { return (ldouble_raw){c}; }

static inline const_char_ptr_raw _to_psn_const_char_ptr(const char *c) { return (const_char_ptr_raw){c}; }
static inline const_char_ptr_raw _to_psn_const_char_arr_ptr(const char (*c)[]) { return (const_char_ptr_raw){*c}; }
static inline const_char_ptr_raw _to_psn_const_char_ptr_bool(bool c) { return (const_char_ptr_raw){ _p_bool(c) }; }

/*** hex pack */

static inline _hex_uchar_raw  _to_psn_hex_uchar(_hex_uchar c)   { return (_hex_uchar_raw){c.v}; }
static inline _hex_ushort_raw _to_psn_hex_ushort(_hex_ushort c) { return (_hex_ushort_raw){c.v}; }
static inline _hex_uint_raw   _to_psn_hex_uint(_hex_uint c)     { return (_hex_uint_raw){c.v}; }
static inline _hex_ulong_raw  _to_psn_hex_ulong(_hex_ulong c)   { return (_hex_ulong_raw){c.v}; }
static inline _hex_ullong_raw _to_psn_hex_ullong(_hex_ullong c) { return (_hex_ullong_raw){c.v}; }

/* Unpack functions */

static inline signed char        _psn_schar(schar_raw c)   { return c.v; }
static inline unsigned char      _psn_uchar(uchar_raw c)   { return c.v; }

static inline short              _psn_sshort(sshort_raw c) { return c.v; }
static inline unsigned short     _psn_ushort(ushort_raw c) { return c.v; }

static inline int                _psn_sint(sint_raw c)     { return c.v; }
static inline unsigned           _psn_uint(uint_raw c)     { return c.v; }

static inline long               _psn_slong(slong_raw c)   { return c.v; }
static inline unsigned long      _psn_ulong(ulong_raw c)   { return c.v; }

static inline long long          _psn_sllong(sllong_raw c) { return c.v; }
static inline unsigned long long _psn_ullong(ullong_raw c) { return c.v; }

static inline float              _psn_float(float_raw c)     { return c.v; }
static inline double             _psn_double(double_raw c)   { return c.v; }
static inline long double        _psn_ldouble(ldouble_raw c) { return c.v; }

static inline const char*        _psn_const_char_ptr(const_char_ptr_raw c){ return c.v; }

/*** unpack hex */

static inline unsigned char       _psn_hex_uchar(_hex_uchar_raw c)  { return c.v; }
static inline unsigned short      _psn_hex_ushort(_hex_ushort_raw c){ return c.v; }
static inline unsigned            _psn_hex_uint(_hex_uint_raw c)    { return c.v; }
static inline unsigned long       _psn_hex_ulong(_hex_ulong_raw c)  { return c.v; }
static inline unsigned long long  _psn_hex_ullong(_hex_ullong_raw c){ return c.v; }

/* convert number to hex with specified precision:
 * println( fmt_hex_p(1, 4) ); -> println( fmt_p(fmt_hex(1), 4) ); -> printf("%.*x\n", 4, 1); */
#define fmt_hex_p(var, prcsn) fmt_p(fmt_hex(var), prcsn)

/* convert number to hex with specified width:
 * println( fmt_hex_w(1, 4) ); -> println( fmt_w(fmt_hex(1), 4) ); -> printf("%*x\n", 4, 1); */
#define fmt_hex_w(var, prcsn) fmt_w(fmt_hex(var), prcsn)

/* convert number to hex with specified width, filled with zeros:
 * println( fmt_hex_zw(1, 4) ); -> println( fmt_zw(fmt_hex(1), 4) ); -> printf("%0*x\n", 4, 1); */
#define fmt_hex_zw(var, prcsn) fmt_zw(fmt_hex(var), prcsn)


#define generic_format_selector_int_fmt(var)    \
    _hex_uchar:         _to_psn_hex_uchar,      \
    _hex_ushort:        _to_psn_hex_ushort,     \
    _hex_uint:          _to_psn_hex_uint,       \
    _hex_ulong:         _to_psn_hex_ulong,      \
    _hex_ullong:        _to_psn_hex_ullong,     \
    \
    signed char:        _to_psn_schar  ,                \
    unsigned char:      _to_psn_uchar  ,                \
    short:              _to_psn_sshort ,                \
    unsigned short:     _to_psn_ushort ,                \
    int:                _to_psn_sint   ,                \
    unsigned:           _to_psn_uint   ,                \
    long:               _to_psn_slong  ,                \
    unsigned long:      _to_psn_ulong  ,                \
    long long:          _to_psn_sllong ,                \
    unsigned long long: _to_psn_ullong                  \

#define generic_format_selector_real_fmt(var) \
    float:              _to_psn_float  ,                \
    double:             _to_psn_double ,                \
    long double:        _to_psn_ldouble                 \

#define generic_format_selector_string_fmt(var) \
    char *:             _to_psn_const_char_ptr,         \
    const char *:       _to_psn_const_char_ptr,         \
    char (*)[]:         _to_psn_const_char_arr_ptr,     \
    const char (*)[]:   _to_psn_const_char_arr_ptr,      \
    bool:               _to_psn_const_char_ptr_bool

#define generic_precision_width_fmt(var) _Generic((var),\
    generic_format_selector_int_fmt(var),               \
    generic_format_selector_real_fmt(var),              \
    generic_format_selector_string_fmt(var)             \
)(var)

#define generic_precision_width_numbers_fmt(var) _Generic((var),\
    generic_format_selector_int_fmt(var),        \
    generic_format_selector_real_fmt(var)        \
)(var)

#define generic_precision_width_real_fmt(var) _Generic((var),\
    generic_format_selector_real_fmt(var)                    \
)(var)


/* WARNING: Do not wrap fmt_x() macros in parentheses!! Use them only inside print*() functions! */

/* precision modifier: printf("%.*d", 4, 10); */
#define fmt_p(var, precision) \
    _pack_precision(precision), generic_precision_width_fmt(var)

/* width modifier: printf("%*d", 4, 10); width can be negative */
#define fmt_w(var, width) \
    _pack_width(width), generic_precision_width_fmt(var)

/* width and precision modifiers: printf("%*.*d", 4, 8, 10); width can be negative */
#define fmt_wp(var, width, precision) \
    _pack_width(width), _pack_and_precision(precision), generic_precision_width_fmt(var)

/* width with zero flag modifier: printf("%0*d", 4, 10); can only be used with numbers */
#define fmt_zw(var, width) \
    _pack_width_zero(width), generic_precision_width_numbers_fmt(var)

/* width with zero flag modifier and precision: printf("%0*.*f", 4, 8, 10f); can be used only with real types */
#define fmt_zwp(var, width, precision) \
    _pack_width_zero(width), _pack_and_precision(precision), generic_precision_width_real_fmt(var)

/* printf_dec_format(variable)
 *
 * Turns standard types into printf's format specifier
 */

#define printf_dec_format(x) printf_dec_format_base(x,)
#define printf_dec_format_newline(x) printf_dec_format_base(x, "\n")

#define printf_dec_format_base(x, after) _Generic((x), \
    _hex_uchar_raw:     "hhx"  ,  \
    _hex_ushort_raw:    "hx"   ,  \
    _hex_uint_raw:      "x"    ,  \
    _hex_ulong_raw:     "lx"   ,  \
    _hex_ullong_raw:    "llx"  ,  \
    \
    schar_raw:          "hhd"  ,  \
    uchar_raw:          "hhu"  ,  \
    sshort_raw:         "hd"   ,  \
    ushort_raw:         "hu"   ,  \
    sint_raw:           "d"    ,  \
    uint_raw:           "u"    ,  \
    slong_raw:          "ld"   ,  \
    ulong_raw:          "lu"   ,  \
    sllong_raw:         "lld"  ,  \
    ullong_raw:         "llu"  ,  \
    float_raw:          "f"    ,  \
    double_raw:         "lf"   ,  \
    ldouble_raw:        "Lf"   ,  \
    const_char_ptr_raw: "s"    ,  \
    \
    format_precision:     "%.*",  \
    format_width:         "%*" ,  \
    format_width_zero:    "%0*",  \
    format_and_precision: ".*" ,  \
    \
    _hex_uchar:         "%hhx" after,            \
    _hex_ushort:        "%hx"  after,            \
    _hex_uint:          "%x"   after,            \
    _hex_ulong:         "%lx"  after,            \
    _hex_ullong:        "%llx" after,            \
    \
    char:               "%c"   after,            \
    signed char:        "%hhd" after,            \
    unsigned char:      "%hhu" after,            \
    short:              "%hd"  after,            \
    unsigned short:     "%hu"  after,            \
    int:                "%d"   after,            \
    unsigned:           "%u"   after,            \
    long:               "%ld"  after,            \
    unsigned long:      "%lu"  after,            \
    long long:          "%lld" after,            \
    unsigned long long: "%llu" after,            \
    float:              "%f"   after,            \
    double:             "%lf"  after,            \
    long double:        "%Lf"  after,            \
    char *:             "%s"   after,            \
    const char *:       "%s"   after,            \
    char (*)[]:         "%s"   after,            \
    const char (*)[]:   "%s"   after,            \
    void *:             "%p"   after,            \
    const void *:       "%p"   after,            \
    bool:               "%s"   after             \
)

#define _each_printf_args(arg) _Generic((arg),   \
        _hex_uchar_raw:         _psn_hex_uchar,      \
        _hex_ushort_raw:        _psn_hex_ushort,     \
        _hex_uint_raw:          _psn_hex_uint,       \
        _hex_ulong_raw:         _psn_hex_ulong,      \
        _hex_ullong_raw:        _psn_hex_ullong,     \
        \
        schar_raw:          _psn_schar,   \
        uchar_raw:          _psn_uchar,   \
        sshort_raw:         _psn_sshort,  \
        ushort_raw:         _psn_ushort,  \
        sint_raw:           _psn_sint,    \
        uint_raw:           _psn_uint,    \
        slong_raw:          _psn_slong,   \
        ulong_raw:          _psn_ulong,   \
        sllong_raw:         _psn_sllong,  \
        ullong_raw:         _psn_ullong,  \
        float_raw:          _psn_float,   \
        double_raw:         _psn_double,  \
        ldouble_raw:        _psn_ldouble, \
        const_char_ptr_raw: _psn_const_char_ptr,    \
        \
        format_precision:    _unpack_precision,     \
        format_width:        _unpack_width,         \
        format_width_zero:   _unpack_width_zero,    \
        format_and_precision:_unpack_and_precision, \
        \
        _hex_uchar:         _p_hex_uchar,       \
        _hex_ushort:        _p_hex_ushort,      \
        _hex_uint:          _p_hex_uint,        \
        _hex_ulong:         _p_hex_ulong,       \
        _hex_ullong:        _p_hex_ullong,      \
        \
        char:               _p_char,            \
        signed char:        _p_schar,           \
        unsigned char:      _p_uchar,           \
        short:              _p_sshort,          \
        unsigned short:     _p_ushort,          \
        int:                _p_sint,            \
        unsigned:           _p_uint,            \
        long:               _p_slong,           \
        unsigned long:      _p_ulong,           \
        long long:          _p_sllong,          \
        unsigned long long: _p_ullong,          \
        float:              _p_float,           \
        double:             _p_double,          \
        long double:        _p_ldouble,         \
        char *:             _p_cchar_ptr,       \
        const char *:       _p_cchar_ptr,       \
        char (*)[]:         _p_cchar_arr_ptr,   \
        const char (*)[]:   _p_cchar_arr_ptr,   \
        void *:             _p_cvoid_ptr,       \
        const void *:       _p_cvoid_ptr,       \
        bool:               _p_bool             \
)((arg))

/* This macro is used to convert values for printing */
#define printf_args_pre_process(...) \
    MAP_LIST(_each_printf_args, __VA_ARGS__)

/*
 * printf_specifier_string(endl, ...):
 * returns static single zero-terminated string with format specifiers for all variables passed
 * @endl: if set to non zero then it will add \n symbol to printf specifiers string
 */
#define printf_specifier_string(endl, ...) \
    IF_SINGLE_ARG(printf_specifier_single, printf_specifier_string_multi, __VA_ARGS__)(endl, __VA_ARGS__)

#define printf_specifier_single(endl, arg) \
    IF( endl ) ( printf_dec_format_newline, printf_dec_format )(arg)

#define _printf_specifier_size(x) (sizeof(printf_dec_format(x)) - 1)

#define _gen_printf_specifier(idx, x)  const char PRIMITIVE_CAT(s, idx) [_printf_specifier_size(x)];
#define _add_endline const char endl;
#define _add_endline_symbol '\n',
#define _nothing

#define printf_spec_size(endl, ...) \
        ( MAP_SEP((+), _printf_specifier_size, __VA_ARGS__) + !!(endl) + 1 )

//#define printf_specifier_string_multi(endl, ...) printf_specifier_string_multi_es(endl, __VA_ARGS__)
#define printf_specifier_string_multi(endl, ...) s_printf_specifier_string_multi_es(endl, __VA_ARGS__)
//#define printf_specifier_string_multi(endl, ...) printf_specifier_string_multi_cl(endl, __VA_ARGS__)
//#define printf_specifier_string_multi(endl, ...) s_printf_specifier_string_multi_cl(endl, __VA_ARGS__)

/* Variant of creating format string with compound literal,
 * it uses automatic storage duration, but no compiler extensions used here, except for __COUNTER__ macro */
#define printf_specifier_string_multi_cl(endl, ...) (           \
(const union {                                                  \
        const struct {                                          \
                MAP_INDEX(_gen_printf_specifier, __VA_ARGS__)   \
                IF(endl)(_add_endline, _nothing)                \
                const char null;                                \
        } specifier ;                                           \
        const char array [printf_spec_size(endl, __VA_ARGS__)]; \
}){                                                             \
        .specifier = {                                          \
                MAP_LIST(printf_dec_format, __VA_ARGS__),       \
                IF(endl)(_add_endline_symbol, _nothing)         \
                0                                               \
        }                                                       \
}                                                               \
).array                                                         \

/* Simplified compound literal variant, struct directly casted to const char* */
#define s_printf_specifier_string_multi_cl(endl, ...)   \
(const char*)&                                          \
(const struct {                                         \
    MAP_INDEX(_gen_printf_specifier, __VA_ARGS__)       \
    IF(endl)(_add_endline, _nothing)                    \
    const char null;                                    \
}){                                                     \
    MAP_LIST(printf_dec_format, __VA_ARGS__),           \
    IF(endl)(_add_endline_symbol, _nothing)             \
    0                                                   \
}                                                       \

/* Variant of creating format string with an expression statement,
 * resulting object generated at compile time  */
#define printf_specifier_string_multi_es(endl, ...) ({          \
static const union {                                            \
        const struct {                                          \
                MAP_INDEX(_gen_printf_specifier, __VA_ARGS__)   \
                IF(endl)(_add_endline, _nothing)                \
                const char null;                                \
        } specifier;                                            \
        const char array [printf_spec_size(endl, __VA_ARGS__)]; \
} generic_printf_format_string = {                              \
        .specifier = {                                          \
                MAP_LIST(printf_dec_format, __VA_ARGS__),       \
                IF(endl)(_add_endline_symbol, _nothing)         \
                0                                               \
        }                                                       \
};                                                              \
static_assert( sizeof(generic_printf_format_string.array) == sizeof(generic_printf_format_string.specifier), \
"Size of structure for generic format string differs from array"); \
generic_printf_format_string.array;                             \
})

/* Simplified expression statement variant, strcut directly casted to const char* */
#define s_printf_specifier_string_multi_es(endl, ...) ({    \
static const struct {                                       \
    MAP_INDEX(_gen_printf_specifier, __VA_ARGS__)           \
    IF(endl)(_add_endline, _nothing)                        \
    const char null;                                        \
} generic_printf_format_string = {                          \
    MAP_LIST(printf_dec_format, __VA_ARGS__),               \
    IF(endl)(_add_endline_symbol, _nothing)                 \
    0                                                       \
};                                                          \
(const char*)&generic_printf_format_string;                 \
})

/*** Print optimization ****/

/* is_same_type(variable, _type_, SIMPLE, CONST)
 * returns true if variable is _type_, or returns false
 *
 * if SIMPLE is not 0, then check for basic _type_ match will be performed
 * if CONST is not 0, then returns true if variable is const _type_
 */
#define is_same_type(var, _type_, SIMPLE, CONST) _Generic((var), \
                        IF(SIMPLE)(simple_type, EAT)(_type_)     \
                        IF(CONST)(const_type, EAT)(_type_)       \
                        default: false)

#define simple_type(_type_) _type_: true,
#define const_type(_type_) const _type_: true,

/* checks if providied value is [const] char *, and returns var if true, or returns NULL if false */
#define char_ptr_or_nullptr(var) _Generic((var), char*: var, const char*:var, default: NULL)

/* puts()/fputs() functions should not receive NULL, if var is NULL then we will return "(null)" string,
 * emulating printf() behavior. But, passing NULL pointer to printf is still undefined behavior. */
static inline const char* check_char_ptr(const char *c) { return c ? c : "(null)"; }
#define puts_charptr_guard(var) check_char_ptr( char_ptr_or_nullptr(var) )

/* returns var if var is char, or returns 0 */
#define char_or_zero(var) _Generic((var), char: var, const char: var, default: 0)

/* single println variant, optimized case when single argument is a string
 * in that case we will use puts() */
#define single_println(arg)                                                 \
        is_same_type(arg, char*, 1, 1) ? puts( puts_charptr_guard(arg) ) :  \
        println_main(arg)

/* single print variant, optimized cases when single argument is string or char */
#define single_print(arg)                                                           \
        is_same_type(arg, char*, 1, 1) ? fputs( puts_charptr_guard(arg), stdout ) : \
        is_same_type(arg, char, 1, 1)  ? putchar( char_or_zero(arg) ) :             \
        print_main(arg)

/* single fprint variant, optimised cases when single argument is string or char */
#define single_fprint(stream, arg)                                                  \
        is_same_type(arg, char*, 1, 1) ? fputs( puts_charptr_guard(arg), stream ) : \
        is_same_type(arg, char, 1, 1)  ? fputc( char_or_zero(arg), stream ) :       \
        fprint_main(stream, arg)

/* These functions are real printx() functions */
#define print_main(...)   printf(printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define println_main(...) printf(printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define fprint_main(stream, ...)   fprintf(stream, printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

/* print[ln]()
 *
 * Prints any number(almost) of any standard variables
 *
 * Basically this is complex wrapper around printf() function.
 * by using _Generic() and some magic this macro will create printf specifier string
 * for printf() at compile time for all arguments passed.
 *
 * for example, look at this code:
 *
   short sh = 5;
   size_t sz = 9;
   intmax_t max = 700;
   bool truth = false;
   const char str[] = "string";
   uint_fast16_t hx = 0xfefe;

   println(sh, sz, max, truth, str, to_hex(hx, 8));

 * println macro after expanding and compilation will become on amd64 linux platform:

   printf("%hd%lu%ld%s%s%.8lx\n", sh, sz, max, "false", str, hx);

 *
 *  variable sz of size_t mapped to unsigned long on this platform - %lu
 *  variable max of intmax_t mapped to long - %ld
 *  variable truth is replaced with string "false", (this is actually runtime check)
 *  specifier for variable hx is %.8lx and uint_fast16_t is actiually unsigned long on this platform
 *
 * So, by using println() you do not need to remember all these specifiers.
 * Just print your variables with println() and don't fuck your brain anymore!
 *
 * usage examples:

     println("this is int:", 45, ", this is float:", 65.14f, " and this is unsigned long to hex:0x", to_hex(9212UL));
     // will print:
     // this is int:45, this is float:65.139999 and this is unsigned long to hex:0x23fc

     println(true, " or ", false, "? That is the question.");
     // will print
     // true or false? That is the question.
 */

/* Print to stdout */
#define print(...)   IF_SINGLE_ARG(single_print, print_main, __VA_ARGS__)(__VA_ARGS__)
#define println(...) IF_SINGLE_ARG(single_println, println_main, __VA_ARGS__)(__VA_ARGS__)

/* Print to FILE* */
#define fprint(stream, ...) IF_SINGLE_ARG(single_fprint, fprint_main, __VA_ARGS__)(stream, __VA_ARGS__)

#define fprintln_(stream, ...) fprintf(stream, printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define fprintln(stream, ...) fprintln_(stream, __VA_ARGS__)

/* Print to stderr */
#define printerr_(...)   fprint(stderr, __VA_ARGS__)
#define printerr(...)   printerr_(__VA_ARGS__)

#define printerrln_(...) fprintln(stderr, __VA_ARGS__)
#define printerrln(...)   printerrln_(__VA_ARGS__)

/* Print to fd */
#define dprint(fd, ...)   dprintf(fd, printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define dprintln(fd, ...) dprintf(fd, printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

/* Print to buffer */
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
    char var_name[snprintf(NULL, 0, fmt___ ## var_name, printf_args_pre_process(__VA_ARGS__)) + 1];        \
    sprintf(var_name, fmt___ ## var_name, printf_args_pre_process(__VA_ARGS__));                           \
    var_name[ sizeof(var_name) - 1  ] = '\0'

/* Same as concat_vla(), but without '\0' byte */
#define concat_vla_nonull(var_name, ...)                                                               \
    const char * const fmt___ ## var_name = printf_specifier_string(0, __VA_ARGS__);                   \
    char var_name[snprintf(NULL, 0, fmt___ ## var_name, printf_args_pre_process(__VA_ARGS__))];        \
    sprintf(var_name, fmt___ ## var_name, printf_args_pre_process(__VA_ARGS__))                        \

#define concat_alloca(...) ({ \
    const char * const fmt = printf_specifier_string(0, __VA_ARGS__); \
    size_t size = snprintf(NULL, 0, fmt, printf_args_pre_process(__VA_ARGS__));             \
    char * string = alloca(size + 1);                              \
    sprintf(string, fmt, printf_args_pre_process(__VA_ARGS__));                             \
    string[size] = '\0';                                           \
    string;                                                        \
})

#define concat(...) ({ \
    const char * const fmt = printf_specifier_string(0, __VA_ARGS__); \
    size_t size = snprintf(NULL, 0, fmt, printf_args_pre_process(__VA_ARGS__));             \
    char * string = malloc(size + 1);                              \
    if(!string)                                                    \
        NULL;                                                      \
    sprintf(string, fmt, printf_args_pre_process(__VA_ARGS__));                             \
    string[size] = '\0';                                           \
    string;                                                        \
})


/* Obsolete */

/* This macro just returns third argument and nothing else */
#define RETURN_THIRD_ARGUMENT(nothing, nothing2, return_value, ...) return_value

/* returns 1 if args are completely empty, or 0 if something is present in __VA_ARGS__, including commas, () or other values
 *
 * we are calling RETURN_THIRD_ARGUMENT with four args, if __VA_ARGS__ is empty,
 * second argument will be removed, so fourth argument(1) becomes thrid and returned */
#define IS_ARGS_EMPTY(x, ...) IS_ARGS_EMPTY__(x)
#define IS_ARGS_EMPTY__(...) RETURN_THIRD_ARGUMENT(nothing, ## __VA_ARGS__, 0, 1 )

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

/* Compatibility */
#define COUNT_PARMS(...)     PP_NARG(__VA_ARGS__)

#define to_hex(var, ...) fmt_hex_p(var, __VA_ARGS__)
#define to_hex_p(var, ...) IF(PP_NARG(__VA_ARGS__))(to_hex_p_multi, to_hex_p_single)(var, __VA_ARGS__)
#define to_hex_p_single(var, ...) fmt_p(fmt_hex(var), 1)
#define to_hex_p_multi(var, prcsn) fmt_p(fmt_hex(var), prcsn)

#endif // MISC_H
