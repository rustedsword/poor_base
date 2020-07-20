/*
 * Copyright (C) 2012 William Swanson
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#define H_ARG_CUT( \
    _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
    _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, \
    _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, \
    _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, \
    _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, \
    N, ...) N

#define H_ARGS_COUNT_SEQ_MAP()                        \
    100, \
    99, 98, 97, 96, 95, 94, 93, 92, 91, 90,           \
    89, 88, 87, 86, 85, 84, 83, 82, 81, 80,           \
    79, 78, 77, 76, 75, 74, 73, 72, 71, 70,           \
    69, 68, 67, 66, 65, 64, 63, 62, 61, 60,           \
    59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
    39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
    9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define H_ARGS_COUNT_SEQ_MAP_ZERO()                   \
    99, 98, 97, 96, 95, 94, 93, 92, 91, 90,           \
    89, 88, 87, 86, 85, 84, 83, 82, 81, 80,           \
    79, 78, 77, 76, 75, 74, 73, 72, 71, 70,           \
    69, 68, 67, 66, 65, 64, 63, 62, 61, 60,           \
    59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
    39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
    9,  8,  7,  6,  5,  4,  3,  2,  1,  0,            \
    0

#define H_ARGS_COUNT(...) H_ARG_CUT(__VA_ARGS__)
/* Returns number of variadic arguments. */
#define ARGS_COUNT(...) H_ARGS_COUNT(__VA_ARGS__, H_ARGS_COUNT_SEQ_MAP())

/* Returns number of variadic arguments starting with zero */
#define ARGS_COUNT_ZERO(...) H_ARGS_COUNT(__VA_ARGS__, H_ARGS_COUNT_SEQ_MAP_ZERO())

#define H_EVAL_SEQ_MAP()                                \
    100, \
    100, 100, 100, 100, 100, 100, 100, 100, 100, 90,    \
    90, 90, 90, 90, 90, 90, 90, 90, 90, 80,             \
    80, 80, 80, 80, 80, 80, 80, 80, 80, 70,             \
    70, 70, 70, 70, 70, 70, 70, 70, 70, 60,             \
    60, 60, 60, 60, 60, 60, 60, 60, 60, 50,             \
    50, 50, 50, 50, 50, 50, 50, 50, 50, 40,             \
    40, 40, 40, 40, 40, 40, 40, 40, 40, 30,             \
    30, 30, 30, 30, 25, 25, 25, 25, 25, 20,             \
    20, 20, 20, 20, 15, 15, 15, 15, 15, 10,             \
    9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define EVAL_ARGS_COUNT(...) H_ARGS_COUNT(__VA_ARGS__, H_EVAL_SEQ_MAP())

#define EVAL_1(...) __VA_ARGS__
#define EVAL_2(...) EVAL_1(__VA_ARGS__)
#define EVAL_3(...) EVAL_1(EVAL_1(__VA_ARGS__))
#define EVAL_4(...) EVAL_2(EVAL_1(__VA_ARGS__))
#define EVAL_5(...) EVAL_3(EVAL_1(__VA_ARGS__))
#define EVAL_6(...) EVAL_4(EVAL_1(__VA_ARGS__))
#define EVAL_7(...) EVAL_5(EVAL_1(__VA_ARGS__))
#define EVAL_8(...) EVAL_6(EVAL_1(__VA_ARGS__))
#define EVAL_9(...) EVAL_7(EVAL_1(__VA_ARGS__))
#define EVAL_10(...) EVAL_8(EVAL_1(__VA_ARGS__))

#define EVAL_15(...) EVAL_10(EVAL_4(__VA_ARGS__))
#define EVAL_20(...) EVAL_15(EVAL_4(__VA_ARGS__))
#define EVAL_25(...) EVAL_20(EVAL_4(__VA_ARGS__))
#define EVAL_30(...) EVAL_25(EVAL_4(__VA_ARGS__))

#define EVAL_40(...) EVAL_30(EVAL_9(__VA_ARGS__))
#define EVAL_50(...) EVAL_40(EVAL_9(__VA_ARGS__))
#define EVAL_60(...) EVAL_50(EVAL_9(__VA_ARGS__))
#define EVAL_70(...) EVAL_60(EVAL_9(__VA_ARGS__))
#define EVAL_80(...) EVAL_70(EVAL_9(__VA_ARGS__))
#define EVAL_90(...) EVAL_80(EVAL_9(__VA_ARGS__))
#define EVAL_100(...) EVAL_90(EVAL_9(__VA_ARGS__))

#define EVAL_MAX(...) EVAL_100(__VA_ARGS__)

/* Choses one of EVAL_X macro depending on arguments count */
#define EVAL_SELECT(...) CAT(EVAL_, EVAL_ARGS_COUNT(__VA_ARGS__))

/* map helpers */
#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)
#define MAP_NEXT(test, next)  MAP_NEXT1(MAP_GET_END test, next)

/************ --- simple map --- **********/
/*
 * Calls macro/function 'f' for each parameters one at a time.
 * example:

   #define do_print(val)  printf("%d\n", val);
   MAP(do_print, 2,8,5)

   //this expands to:
   // printf("%d\n", 2);
   // printf("%d\n", 8);
   // printf("%d\n", 5);

 */
#define MAP(f, ...) EVAL_SELECT(__VA_ARGS__)(MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)

/*
 * Calls macro/function 'f' for each parameters one at a time and with parameter 'p'
 * example:

    int a[] = {4, 7, 8, 9, 5};

    #define do_print(arg, val)  printf("%d\n", arg[val]);
    MAP_ARG(do_print, a, 3, 1, 0)

    //this expands to:
    // printf("%d\n", a[3]);
    // printf("%d\n", a[1]);
    // printf("%d\n", a[0]);

 */
#define MAP_ARG(f, p, ...) EVAL_SELECT(__VA_ARGS__)(MAP_ARG1(f, p, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_ARG0(f, p, x, peek, ...) f(p, x) MAP_NEXT(peek, MAP_ARG1)(f, p, peek, __VA_ARGS__)
#define MAP_ARG1(f, p, x, peek, ...) f(p, x) MAP_NEXT(peek, MAP_ARG0)(f, p, peek, __VA_ARGS__)

/*
 * Calls macro/function 'f' for each parameters and provides index of each parameter starting from 0
 * example:

    #define do_print(idx, val)  printf("index:%d, value:%d\n", idx, val);
    MAP_INDEX(do_print, 5, 7, 15)

    //this expands to:
    // printf("index:%d, value:%d\n", 0, 5);
    // printf("index:%d, value:%d\n", 1, 7);
    // printf("index:%d, value:%d\n", 2, 15);

 */
#define MAP_INDEX(f, ...) \
    EVAL_SELECT(__VA_ARGS__)(MAP_INDEX1(f, (1), __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_INDEX0(f, cnt, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_NEXT(peek, MAP_INDEX1)(f, (EXPAND cnt, 1), peek, __VA_ARGS__)
#define MAP_INDEX1(f, cnt, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_NEXT(peek, MAP_INDEX0)(f, (EXPAND cnt, 1), peek, __VA_ARGS__)

/*
 * Calls macro/function 'f' for each parameters, provides index of each parameter starting from 0 and passes parameter 'p'
 * example:

    int a[4];
    #define do_init(idx, arg, val)  arg[idx] = val;
    MAP_ARG_INDEX(do_init, a, 2, 7, 8, 13)

    //this expands to:
    // a[0] = 2;
    // a[1] = 7;
    // a[2] = 8;
    // a[3] = 13;

 */
#define MAP_ARG_INDEX(f, p, ...) \
    EVAL_SELECT(__VA_ARGS__)(MAP_ARG_INDEX1(f, (1), p, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_ARG_INDEX0(f, cnt, p, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), p, x) MAP_NEXT(peek, MAP_ARG_INDEX1)(f, (EXPAND cnt, 1), p, peek, __VA_ARGS__)
#define MAP_ARG_INDEX1(f, cnt, p, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), p, x) MAP_NEXT(peek, MAP_ARG_INDEX0)(f, (EXPAND cnt, 1), p, peek, __VA_ARGS__)

/************ --- map with separator --- **********/
/* Helpers */
#define MAP_SEP_NEXT1(test, sep, next) MAP_NEXT0(test, EXPAND sep next, 0)
#define MAP_SEP_NEXT(test, sep, next)  MAP_SEP_NEXT1(MAP_GET_END test, sep, next)

/*
 * Calls macro/function 'f' for each parameters and inserts 'sep' as separator between invocations
 * 'sep' must be enclosed in parentheses
 * example:

    #define do_add(val)  (val + 2)
    int a = MAP_SEP((+), do_add, 2, 4, 3);

    //this expands to:
    // int a = (2 + 2) + (4 + 2) + (3 + 2);

    #define do_print_value(val) "val:", val
    println("Values: ", MAP_SEP((, " ",), do_print_value, 40, 5, 2) );

    //this expands to:
    // println("Values: ", "val:", 40, " ", "val:", 5, " ", "val:", 2);

 */
#define MAP_SEP(sep, f, ...) EVAL_SELECT(__VA_ARGS__)(MAP_SEP1(sep, f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_SEP0(sep, f, x, peek, ...) f(x) MAP_SEP_NEXT(peek, sep, MAP_SEP1)(sep, f, peek, __VA_ARGS__)
#define MAP_SEP1(sep, f, x, peek, ...) f(x) MAP_SEP_NEXT(peek, sep, MAP_SEP0)(sep, f, peek, __VA_ARGS__)

/*
 * Calls macro/function 'f' for each parameters and inserts 'sep' as separator between invocations.
 * passes parameter 'p' as first argument
 */
#define MAP_SEP_ARG(sep, f, p, ...) EVAL_SELECT(__VA_ARGS__)(MAP_SEP_ARG1(sep, f, p, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_SEP_ARG0(sep, f, p, x, peek, ...) f(p,x) MAP_SEP_NEXT(peek, sep, MAP_SEP_ARG1)(sep, f, p, peek, __VA_ARGS__)
#define MAP_SEP_ARG1(sep, f, p, x, peek, ...) f(p,x) MAP_SEP_NEXT(peek, sep, MAP_SEP_ARG0)(sep, f, p, peek, __VA_ARGS__)

/*
 * Calls macro/function 'f' for each parameters and inserts 'sep' as separator between invocations.
 * passes index as first argument
 */
#define MAP_SEP_IDX(sep, f, ...) EVAL_SELECT(__VA_ARGS__)(MAP_SEP_IDX1(sep, f, (1), __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_SEP_IDX0(sep, f, cnt, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_SEP_NEXT(peek, sep, MAP_SEP_IDX1)(sep, f, (EXPAND cnt, 1), peek, __VA_ARGS__)
#define MAP_SEP_IDX1(sep, f, cnt, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_SEP_NEXT(peek, sep, MAP_SEP_IDX0)(sep, f, (EXPAND cnt, 1), peek, __VA_ARGS__)

/*
 * Calls macro/function 'f' for each parameters and inserts 'sep' as separator between invocations.
 * passes index as first argument
 * passes 'p' as second argument
 * example:

    int a[10];

    #define init_array_element(_idx_, _array_, _val_) (_array_)[_idx_] = (_val_)
    ( MAP_SEQ_ARG_IDX((,), init_array_element, a, 10, 20, 30) );

    //this expands to:
    // ( a[0] = 10, a[1] = 20, a[2] = 30 );

 */
#define MAP_SEP_ARG_IDX(sep, f, p, ...) EVAL_SELECT(__VA_ARGS__)(MAP_SEP_ARG_IDX1(sep, f, (1), p, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_SEP_ARG_IDX0(sep, f, cnt, p, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), p, x) MAP_SEP_NEXT(peek, sep, MAP_SEP_ARG_IDX1)(sep, f, (EXPAND cnt, 1), p, peek, __VA_ARGS__)
#define MAP_SEP_ARG_IDX1(sep, f, cnt, p, x, peek, ...) \
    f(ARGS_COUNT_ZERO(EXPAND cnt), p, x) MAP_SEP_NEXT(peek, sep, MAP_SEP_ARG_IDX0)(sep, f, (EXPAND cnt, 1), p, peek, __VA_ARGS__)

/****** ---- Map list ---- ********/
#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next)  MAP_LIST_NEXT1(MAP_GET_END test, next)

/*
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 * It is basically same as MAP_SEP((,), f, ...)
 */
#define MAP_LIST(f, ...) EVAL_SELECT(__VA_ARGS__)(MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_LIST0(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST1)(f, peek, __VA_ARGS__)
#define MAP_LIST1(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST0)(f, peek, __VA_ARGS__)

/*
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 * It is basically same as MAP_SEP_ARG((,), f, p, ...)
 */
#define MAP_LIST_ARG(f, p, ...) EVAL_SELECT(__VA_ARGS__)(MAP_LIST_ARG1(f, p, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_LIST_ARG0(f, p, x, peek, ...) f(p, x) MAP_LIST_NEXT(peek, MAP_LIST_ARG1)(f, p, peek, __VA_ARGS__)
#define MAP_LIST_ARG1(f, p, x, peek, ...) f(p, x) MAP_LIST_NEXT(peek, MAP_LIST_ARG0)(f, p, peek, __VA_ARGS__)


/* Obsolete */

/*
 * Applies the function macro `f` to first argument and each of the remaining parameters and
 * inserts commas between the results.
 */
#define MAP_LIST_TWOARG(f, p1, p2, ...) \
    EVAL_SELECT(__VA_ARGS__)(MAP_LIST_TWOARG1(f, p1, p2, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_LIST_TWOARG0(f, p1, p2, x, peek, ...) f(p1, p2, x) MAP_LIST_NEXT(peek, MAP_LIST_TWOARG1)(f, p1, p2, peek, __VA_ARGS__)
#define MAP_LIST_TWOARG1(f, p1, p2, x, peek, ...) f(p1, p2, x) MAP_LIST_NEXT(peek, MAP_LIST_TWOARG0)(f, p1, p2, peek, __VA_ARGS__)

/*
 * Applies the function macro `f` to first two arguments and each of the remaining parameters.
 */
#define MAP_TWOARG(f, p1, p2, ...) \
    EVAL_SELECT(__VA_ARGS__)(MAP_TWOARG1(f, p1, p2, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_TWOARG0(f, p1, p2, x, peek, ...) f(p1, p2, x) MAP_NEXT(peek, MAP_TWOARG1)(f, p1, p2, peek, __VA_ARGS__)
#define MAP_TWOARG1(f, p1, p2, x, peek, ...) f(p1, p2, x) MAP_NEXT(peek, MAP_TWOARG0)(f, p1, p2, peek, __VA_ARGS__)

/*
 * Applies the function macro `f` to each pair of the remaining parameters.
 */
#define MAP_TWO(f, ...) EVAL_MAX(MAP_TWO1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_TWO0(f, x, y, peek1, peek2, ...) f(x, y) MAP_NEXT(peek2, MAP_TWO1)(f, peek1, peek2, __VA_ARGS__)
#define MAP_TWO1(f, x, y, peek1, peek2, ...) f(x, y) MAP_NEXT(peek2, MAP_TWO0)(f, peek1, peek2, __VA_ARGS__)


#endif
