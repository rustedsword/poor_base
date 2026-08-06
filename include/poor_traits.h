// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2020 Alexandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_TRAITS_H
#define POOR_TRAITS_H
#include <inttypes.h>
#include <assert.h>

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

/* is_vla(array)
 * Returns true if array is variable length array(VLA)
 * This macro can be used with any type of variable.
 */
#define is_vla(arr) (!is_const_expr(sizeof(arr)))

/* Evaluates (t) expression if arr is VLA, otherwise evaluates (f) expression
 * This macro can be used with any type of variable. */
#define if_vla(arr, t, f) if_constexpr(sizeof(arr), (f), (t))


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

#endif // POOR_TRAITS_H
