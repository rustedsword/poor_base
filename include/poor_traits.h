// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2020 Alexandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_TRAITS_H
#define POOR_TRAITS_H
#include <poor_map.h>
#include <inttypes.h>
#include <assert.h>
#include <stddef.h>

#if !defined __cplusplus && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 202311L)
# error "poor_base requires C23 (compile with -std=c23 or -std=gnu23)"
#endif

/* static_assert that can be used inside other expressions
 * example:

    //assign int a = 5 only if size of int equals 4 or terminate compilation
    int a = (static_assert_expr(sizeof(int) == 4, "Size of int should be 4!"), 5);

*/
#define static_assert_expr(expr, message) \
            (void)((struct {static_assert( expr , message); int a;}*)NULL)

/* Returns true if ptr points to something with const qualifier
 *
 * This macro can be used only with pointers or arrays (array decayed to pointer) */
#define is_pointer_to_const(ptr) _Generic((1 ? ptr : (void*)1),   \
        const void *: true,                                         \
        const volatile void*: true,                                 \
        default: false )                                            \

/* Returns true if ptr points to something with volatile qualifier
 *
 * This macro can be used only with pointers or arrays (array decayed to pointer) */
#define is_pointer_to_volatile(ptr) _Generic((1 ? ptr : (void*)1), \
        volatile void *: true,                                       \
        const volatile void*: true,                                  \
        default: false )                                             \

/* returns pointer to int if (expr) is constant integer expression,
 * or returns pointer to void if (expr) is not constant integer expression */
#define magic_ice_expression(expr) (1 ? ((void *)((intptr_t)( (expr) ) * 0)) : (int *)1)

/* returns true if (expr) is constant integer expression, or false if it is not */
#define is_const_expr(expr) _Generic( magic_ice_expression(expr), int*: true, void*:false)

/* Evaluates (if_const) expression if (expr) is constant integer expression
 * otherwise evaluates (if_not_const) expression */
#define if_constexpr(expr, if_const, if_not_const) _Generic( magic_ice_expression(expr), int*: (if_const), void*:(if_not_const))

/* returns (x) if it is constant integer expression, or (d) if it is not */
#define constexpr_or(x, d) if_constexpr(x, x, d)

/* is_vla(array)
 * Returns true if array is variable length array(VLA)
 * This macro can be used with any type of variable.
 */
#define is_vla(arr) (!is_const_expr(sizeof(arr)))

/* Evaluates (t) expression if arr is VLA, otherwise evaluates (f) expression
 * This macro can be used with any type of variable. */
#define if_vla(arr, t, f) if_constexpr(sizeof(arr), (f), (t))

#define is_unsigned(x) constexpr_or((typeof((void)0, (x)))-1 > 0, 0)

/* Evaluates (t) expression if x has an unsigned integer type, otherwise evaluates (f) expression. */
#define if_unsigned(x, t, f) _Generic((char (*)[1 + is_unsigned(x)])0, char (*)[2]: (t), default: (f))


/* returns true if var is subject to the default argument promotions.
 * i.e bool, char, unsigned char, signed char, short, unsigned short are promoted to int
 * and float is promoted to double
 *
 * This macro can be used with any type of variable.
 */
#define is_promotable(var) _Generic( (var), \
        bool:               true,          \
        char:               true,          \
        signed char:        true,          \
        unsigned char:      true,          \
        short:              true,          \
        unsigned short:     true,          \
        float:              true,          \
        default:            false )

/* is_same_type(variable, _type_, SIMPLE, CONST)
 * returns true if variable is _type_, or returns false
 *
 * if SIMPLE is not 0, then check for basic _type_ match will be performed
 * if CONST is not 0, then returns true if variable is const _type_
 */
#define is_same_type(var, _type_, SIMPLE, CONST) _Generic((var), \
                        POOR_IF(SIMPLE)(h_same_type_simple, POOR_EAT)(_type_)   \
                        POOR_IF(CONST)(h_same_type_const, POOR_EAT)(_type_)     \
                        default: false)

#define h_same_type_simple(_type_) _type_: true,
#define h_same_type_const(_type_) const _type_: true,

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
                                         default: h_container_of(ptr, type, member) )
#define h_container_of(ptr, type, member) (type *)( (uintptr_t)(ptr) - offsetof(type, member) )

#endif // POOR_TRAITS_H
