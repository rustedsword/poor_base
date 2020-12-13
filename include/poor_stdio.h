// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2020 Alexandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_STDIO_H
#define POOR_STDIO_H

#include <poor_array.h>
#include <poor_map.h>
#include <poor_traits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define BITAND(x) PRIMITIVE_CAT(BITAND_, x)
#define BITAND_0(y) 0
#define BITAND_1(y) y

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


/**** ---- printf format specifiers helper functions ****/
/* unpack basic types */

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

/* This is special function for bool type to print strings "true" or "false", instead of 1 or 0,
 * since there is no printf format specifier for bool */
static inline const char *       _p_bool(bool c) { return c ? "true" : "false"; }

/* hex types */
typedef union { unsigned char v; }      _hex_uchar;
typedef union { unsigned short v; }     _hex_ushort;
typedef union { unsigned v; }           _hex_uint;
typedef union { unsigned long v; }      _hex_ulong;
typedef union { unsigned long long v; } _hex_ullong;

/* functions to pack unsigned integers into _hex unions for _Generic selection  */
static inline _hex_uchar  _to_hex_uchar(unsigned char c)       { return (_hex_uchar){c}; }
static inline _hex_ushort _to_hex_ushort(unsigned short c)     { return (_hex_ushort){c}; }
static inline _hex_uint   _to_hex_uint(unsigned c)             { return (_hex_uint){c}; }
static inline _hex_ulong  _to_hex_ulong(unsigned long c)       { return (_hex_ulong){c}; }
static inline _hex_ullong _to_hex_ullong(unsigned long long c) { return (_hex_ullong){c}; }

/* functions for unpacking unsigned integers from _hex_types for passing to printf() */
static inline unsigned char       _p_hex_uchar(_hex_uchar c){ return c.v; }
static inline unsigned short      _p_hex_ushort(_hex_ushort c){ return c.v; }
static inline unsigned            _p_hex_uint(_hex_uint c){ return c.v; }
static inline unsigned long       _p_hex_ulong(_hex_ulong c){ return c.v; }
static inline unsigned long long  _p_hex_ullong(_hex_ullong c){ return c.v; }

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

/* pack functions to use with format modifiers  */

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

/*** hex pack to use with format modifiers  */

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


/****** ---- printf modifiers macros ---- ******/

/* fmt_hex(var): hexademical modifier, only integers are supported

	println(fmt_hex(1223)); // -> printf("%x\n", 1223);
	//prints: 4c7
 */
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

/* fmt_hex_p(var, precision) hexademical modifier with specified precision:

	println(fmt_hex_p(1223, 8)); // -> println( fmt_p(fmt_hex(1223), 8) ); -> printf("%.*x\n", 8, 1223);
	//prints: 000004c7
 */
#define fmt_hex_p(var, prcsn) fmt_p(fmt_hex(var), prcsn)

/* fmt_hex_w(var, width) hexademical modifier with specified width:

	println(fmt_hex_w(1223, 8)); // -> println( fmt_w(fmt_hex(1223), 8) ); -> printf("%*x\n", 8, 1223);
	//prints:      4c7
 */
#define fmt_hex_w(var, prcsn) fmt_w(fmt_hex(var), prcsn)

/* fmt_hex_zw(var, width) hexademical modifier with specified width, filled with zeros:

	//println(fmt_hex_zw(1223, 8)); // -> println( fmt_zw(fmt_hex(1223), 8) ); -> printf("%0*x\n", 8, 1223);
	//prints: 000004c7
 */
#define fmt_hex_zw(var, prcsn) fmt_zw(fmt_hex(var), prcsn)

/* fmt_p(var, precision): precision modifier:
 *
	println(fmt_p(10, 4)); // -> printf("%.*d", 4, 10);
	//prints: 0010

	println(fmt_p("string", 4)); // -> printf("%.*s", 4, "string");
	//prints: stri

	println(fmt_p(10.f, 3)); // -> printf("%.*f", 3, 10.f);
	//prints: 10.000

 */
#define fmt_p(var, precision) _pack_precision(precision), generic_precision_width_fmt(var)

/* fmt_w(var, width) width modifier.
 * @width can be negative

	println(fmt_w(10, 9)); // -> printf("%*d", 9, 10);
	//prints:        10

	println(fmt_w("string", 9)); // -> printf("%*s", 9, "string");
	//prints:    string

	println(fmt_w(10.f, 20)); // -> printf("%*s", 20, 10.f);
	//prints:            10.000000

 */
#define fmt_w(var, width) _pack_width(width), generic_precision_width_fmt(var)

/* fmt_wp(var, width, precision) width and precision modifiers:
 * @width can be negative

	println(fmt_wp(10, 9, 5)); // -> printf("%*.*d", 9, 5, 10);
	//prints:     00010

	println(fmt_wp("string", 9, 4)); // -> printf("%*.*s", 9, 5, "string");
	//prints:      stri

	println(fmt_wp(10.f, 8, 2)); // -> printf("%*.*f", 8, 2, 10.f);
	//prints:    10.00
 */
#define fmt_wp(var, width, precision) \
    _pack_width(width), _pack_and_precision(precision), generic_precision_width_fmt(var)

/* fmt_zw(var, width): width with zero flag modifier:
 * can only be used with numbers

	println(fmt_zw(10, 9)); // ->  printf("%0*d", 9, 10);
	//prints: 000000010

	println(fmt_zw(10.f, 20)); // ->  printf("%0*f", 9, 10);
	//prints: 0000000000010.000000
 */
#define fmt_zw(var, width) \
    _pack_width_zero(width), generic_precision_width_numbers_fmt(var)

/* fmt_zwp(var, width, precision) width with zero flag modifier and precision:
 * can only be used with real types

	println(fmt_zwp(10.f, 10, 1)); // -> printf("%0*.*f", 10, 1, 10.f);
	//prints: 00000010.0
 */
#define fmt_zwp(var, width, precision) \
    _pack_width_zero(width), _pack_and_precision(precision), generic_precision_width_real_fmt(var)

/* printf_dec_format(var): returns single printf's format specifier according to type of var */
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

/* printf_args_pre_process(...)
 * This macro should be used in place of variadic arguments of printf-like functions
 * if printf_specifier_string() is used to create format specifers
 * see print_main() macro for example */
#define printf_args_pre_process(...) MAP_LIST(_each_printf_args, __VA_ARGS__)

/*
 * printf_specifier_string(endl, ...):
 * returns static single null-terminated string with format specifiers for all variables passed
 * @endl: if set to non zero then it will add \n symbol to printf specifiers string
 *
 * usage example:
 *	char *format = printf_specifier_string(1, 5.6f, true, fmt_hex(0xFFF));
 *	//format is %f%s%x\n
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
 * it uses automatic storage duration, but no compiler extensions used here */
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
 * resulting string will be generated at compile time, but expression statements is not a part of standard C */
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
 * example:

   short sh = 5;
   size_t sz = 9;
   intmax_t max = 700;
   bool truth = false;
   const char str[] = "string";
   uint_fast16_t hx = 0xfefe;

   println(sh, sz, max, truth, str, fmt_hex_p(hx, 8));

 * println macro after expanding and compilation will become on amd64 linux platform:

   printf("%hd%lu%ld%s%s%.8lx\n", sh, sz, max, "false", str, hx);

 *
 *  variable sz of size_t mapped to unsigned long on this platform - %lu
 *  variable max of intmax_t mapped to long - %ld
 *  variable truth is replaced with string "false", (this is actually runtime check)
 *  specifier for variable hx is %.*lx with precision 8 and uint_fast16_t is actiually unsigned long on this platform
 *
 * So, by using println() you do not need to remember all these specifiers.
 * Just print your variables with println() and don't fuck your brain anymore!
 *
 * usage examples:

     println("this is int:", 45, ", this is float:", 65.14f, " and this is unsigned long to hex:0x", fmt_hex(9212UL));
     // will print:
     // this is int:45, this is float:65.139999 and this is unsigned long to hex:0x23fc

     println(true, " or ", false, "? That is the question.");
     // will print:
     // true or false? That is the question.
 */

/* print(...): print to stdout */
#define print(...)   IF_SINGLE_ARG(single_print, print_main, __VA_ARGS__)(__VA_ARGS__)
/* println(...): print to stdout with newline */
#define println(...) IF_SINGLE_ARG(single_println, println_main, __VA_ARGS__)(__VA_ARGS__)

/* fprint(stream, ...): print to FILE* */
#define fprint(stream, ...) IF_SINGLE_ARG(single_fprint, fprint_main, __VA_ARGS__)(stream, __VA_ARGS__)

/* fprint(stream, ...): print to FILE* with newline */
#define fprintln(stream, ...) h_fprintln_(stream, __VA_ARGS__)
#define h_fprintln_(stream, ...) fprintf(stream, printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

/* printerr(...): print to stderr */
#define printerr(...)   h_printerr_(__VA_ARGS__)
#define h_printerr_(...)   fprint(stderr, __VA_ARGS__)

/* printerrln(...): print to stderr with newline */
#define printerrln(...)   h_printerrln_(__VA_ARGS__)
#define h_printerrln_(...) fprintln(stderr, __VA_ARGS__)

/* dprint(fd, ...): print to file descriptor */
#define dprint(fd, ...)   dprintf(fd, printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
/* dprintln(fd, ...): print to file descriptor with new line*/
#define dprintln(fd, ...) dprintf(fd, printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

/* sprint(_buf_, var1, ..., varn):
 * print to buffer. buffer should be large enough for string to fit
 * returns number of bytes in the printed string without last '\0'
 * always writes '\0' to buf
 *
 * @_buf_: a pointer to char. (pointer to first element in char array)
 *
 * note: it is better to not use this macro, since it is very hard to not overflow buf.
 */
#define sprint(_buf_, ...) sprintf(_buf_, printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
/* sprintln(buf, ...): sprint() with a line end */
#define sprintln(_buf_, ...) sprintf(_buf_, printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

/* sprint_array(_arrm_, var1, ..., varn):
 * prints to char array.
 * returns number of bytes in the printed string without last '\0'
 * always writes '\0' at the end of printed string.
 * if printed string was larger than array, then writes '\0' to the last array element.
 *
 * if returned value is equal or greater than size of the provided array, then output was truncated.
 *
 * @_arrm_: a char array or a pointer to a char array
 * @_var_: standard C types variables that supported by print() macro family
 * example:

	char buf[5];
	if((size_t)sprint_array(buf,1,2,3,4,5) >= sizeof(buf))
		printerrln("Output truncated");

	println(buf); //prints:1234
 */
#define sprint_array(_arrm_, ...) snprintf(auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), printf_specifier_string(0, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))
#define sprintln_array(_arrm_, ...) snprintf(auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), printf_specifier_string(1, __VA_ARGS__), printf_args_pre_process(__VA_ARGS__))

/* concat_vla(_name_, var1, ..., varn)
 * Creates variable length array (_name_) and
 * writes concatenated string into it with '\0' at the end.
 *
 * @_name_: name of a new VLA to declare.
 * @_var_: standard C types variables that supported by print() macro family
 *
 * example:

	int num = 2; uint64_t l = 500;
	concat_vla(vla, "num is:", num, " var l is:", l);

	println("VLA string: ", vla, ". it's size:", sizeof(vla));
	prints: //VLA string: num is:2 var l is:500. it's size:22
*/
#define concat_vla(_name_, ...)										\
	const char * const fmt___ ## _name_ = printf_specifier_string(0, __VA_ARGS__);			\
	char _name_[1 + snprintf(NULL, 0, fmt___ ## _name_, printf_args_pre_process(__VA_ARGS__))];	\
	sprintf(_name_, fmt___ ## _name_, printf_args_pre_process(__VA_ARGS__))

/* concat_malloc_array(_name_, var1, ..., varn):
 * Creates a pointer (_name_) to a variable length array,
 * allocates memory for it and writes concatenated string into that memory with '\0' at the end.
 *
 * if memory allocation fails (_name_) will be a NULL pointer.
 *
 * @_name_: name of new pointer to array to declare.
 * @_var_: standard C types variables that supported by print() macro family
 * example:

	concat_malloc_array(string, "One:", 1L, " Two:", 2ULL);
	if(string) {
		println("result:", string, ". size of that string:", sizeof(*string));
		//result:One:1 Two:2. size of that string:12

		free(string);
	}
 */
#define concat_malloc_array(_name_, ...) \
	const char * const fmt___ ## _name_ = printf_specifier_string(0, __VA_ARGS__);				\
	char (*_name_)[1 + snprintf(NULL, 0, fmt___ ## _name_, printf_args_pre_process(__VA_ARGS__))];		\
	((_name_ = malloc(sizeof(*_name_))) ? sprintf(*_name_, fmt___ ## _name_, printf_args_pre_process(__VA_ARGS__)) : (void)0)

/* concat(var1, ..., varn):
 * Returns a pointer to char with memory allocated by malloc with concatenated string within.
 * Returned string is null-terminated.
 *
 * returns NULL pointer if memory allocation fails.
 *
 * @_var_: standard C types variables that supported by print() macro family
 * example:

	char *buf = concat(1, 2U, 3L, "?");
	if(buf) {
		println(buf);
		//prints: 123?

		free(buf);
	}
 */
#define concat(...) __extension__ ({					\
	concat_malloc_array(_tmp_s_array_, __VA_ARGS__);		\
	(char *)_tmp_s_array_;						\
})


#endif // POOR_STDIO_H
