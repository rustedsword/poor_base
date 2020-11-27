// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2020 Alexandrov Stanislav <lightofmysoul@gmail.com>
 * Copyright (C) 2012 William Swanson
 */

#ifndef POOR_MAP_H_INCLUDED
#define POOR_MAP_H_INCLUDED

#define TOKEN_CAT_2(a, ...) TOKEN_CAT_1(a, __VA_ARGS__)
#define TOKEN_CAT_1(a, ...) a ## __VA_ARGS__

#define EAT(...)
#define EXPAND(...) __VA_ARGS__

#define STRINGIFY_(var) #var
#define STRINGIFY2(var) STRINGIFY_(var)

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
    _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, \
    _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, \
    _121, _122, _123, _124, _125, _126, _127, _128, _129, _130, \
    _131, _132, _133, _134, _135, _136, _137, _138, _139, _140, \
    _141, _142, _143, _144, _145, _146, _147, _148, _149, _150, \
    _151, _152, _153, _154, _155, _156, _157, _158, _159, _160, \
    _161, _162, _163, _164, _165, _166, _167, _168, _169, _170, \
    _171, _172, _173, _174, _175, _176, _177, _178, _179, _180, \
    _181, _182, _183, _184, _185, _186, _187, _188, _189, _190, \
    _191, _192, _193, _194, _195, _196, _197, _198, _199, _200, \
    _201, _202, _203, _204, _205, _206, _207, _208, _209, _210, \
    N, ...) N

#define H_ARGS_COMMON_MAP \
    209, 208, 207, 206, 205, 204, 203, 202, 201, 200, \
    199, 198, 197, 196, 195, 194, 193, 192, 191, 190, \
    189, 188, 187, 186, 185, 184, 183, 182, 181, 180, \
    179, 178, 177, 176, 175, 174, 173, 172, 171, 170, \
    169, 168, 167, 166, 165, 164, 163, 162, 161, 160, \
    159, 158, 157, 156, 155, 154, 153, 152, 151, 150, \
    149, 148, 147, 146, 145, 144, 143, 142, 141, 140, \
    139, 138, 137, 136, 135, 134, 133, 132, 131, 130, \
    129, 128, 127, 126, 125, 124, 123, 122, 121, 120, \
    119, 118, 117, 116, 115, 114, 113, 112, 111, 110, \
    109, 108, 107, 106, 105, 104, 103, 102, 101, 100, \
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


#define H_ARGS_COUNT_SEQ_MAP()      \
    210, \
    H_ARGS_COMMON_MAP

#define H_ARGS_COUNT_SEQ_MAP_ZERO() \
    H_ARGS_COMMON_MAP,              \
    0

#define H_ARGS_COUNT(...) H_ARG_CUT(__VA_ARGS__)
/* Returns number of variadic arguments. */
#define ARGS_COUNT(...) H_ARGS_COUNT(__VA_ARGS__, H_ARGS_COUNT_SEQ_MAP())

/* Returns number of variadic arguments starting with zero */
#define ARGS_COUNT_ZERO(...) H_ARGS_COUNT(__VA_ARGS__, H_ARGS_COUNT_SEQ_MAP_ZERO())

#define H_EVAL_SEQ_MAP()                                \
    210, \
    210, 210, 210, 210, 210, 210, 210, 210, 210, 200,    \
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200,    \
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200,    \
    200, 200, 200, 200, 175, 175, 175, 175, 175, 175,    \
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175,    \
    \
    175, 175, 175, 175, 175, 175, 175, 175, 175, 150,    \
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150,    \
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150,    \
    150, 150, 150, 150, 125, 125, 125, 125, 125, 125,    \
    125, 125, 125, 125, 125, 125, 125, 125, 125, 125,    \
    125, 125, 125, 125, 125, 125, 125, 125, 125, 100,    \
    \
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

#define EVAL_125(...) EVAL_100(EVAL_20(EVAL_4(__VA_ARGS__)))
#define EVAL_150(...) EVAL_100(EVAL_40(EVAL_9(__VA_ARGS__)))
#define EVAL_175(...) EVAL_100(EVAL_70(EVAL_4(__VA_ARGS__)))
#define EVAL_200(...) EVAL_100(EVAL_90(EVAL_9(__VA_ARGS__)))
#define EVAL_210(...) EVAL_200(EVAL_9(__VA_ARGS__))

#define EVAL_MAX(...) EVAL_210(__VA_ARGS__)


/* Same set as above, but with different name, for nested calls */
#define EVAL2_1(...) __VA_ARGS__
#define EVAL2_2(...) EVAL2_1(__VA_ARGS__)
#define EVAL2_3(...) EVAL2_1(EVAL2_1(__VA_ARGS__))
#define EVAL2_4(...) EVAL2_2(EVAL2_1(__VA_ARGS__))
#define EVAL2_5(...) EVAL2_3(EVAL2_1(__VA_ARGS__))
#define EVAL2_6(...) EVAL2_4(EVAL2_1(__VA_ARGS__))
#define EVAL2_7(...) EVAL2_5(EVAL2_1(__VA_ARGS__))
#define EVAL2_8(...) EVAL2_6(EVAL2_1(__VA_ARGS__))
#define EVAL2_9(...) EVAL2_7(EVAL2_1(__VA_ARGS__))
#define EVAL2_10(...) EVAL2_8(EVAL2_1(__VA_ARGS__))

#define EVAL2_15(...) EVAL2_10(EVAL2_4(__VA_ARGS__))
#define EVAL2_20(...) EVAL2_15(EVAL2_4(__VA_ARGS__))
#define EVAL2_25(...) EVAL2_20(EVAL2_4(__VA_ARGS__))
#define EVAL2_30(...) EVAL2_25(EVAL2_4(__VA_ARGS__))

#define EVAL2_40(...) EVAL2_30(EVAL2_9(__VA_ARGS__))
#define EVAL2_50(...) EVAL2_40(EVAL2_9(__VA_ARGS__))
#define EVAL2_60(...) EVAL2_50(EVAL2_9(__VA_ARGS__))
#define EVAL2_70(...) EVAL2_60(EVAL2_9(__VA_ARGS__))
#define EVAL2_80(...) EVAL2_70(EVAL2_9(__VA_ARGS__))
#define EVAL2_90(...) EVAL2_80(EVAL2_9(__VA_ARGS__))
#define EVAL2_100(...) EVAL2_90(EVAL2_9(__VA_ARGS__))

#define EVAL2_125(...) EVAL2_100(EVAL2_20(EVAL2_4(__VA_ARGS__)))
#define EVAL2_150(...) EVAL2_100(EVAL2_40(EVAL2_9(__VA_ARGS__)))
#define EVAL2_175(...) EVAL2_100(EVAL2_70(EVAL2_4(__VA_ARGS__)))
#define EVAL2_200(...) EVAL2_100(EVAL2_90(EVAL2_9(__VA_ARGS__)))
#define EVAL2_210(...) EVAL2_200(EVAL2_9(__VA_ARGS__))

#define EVAL2_MAX(...) EVAL2_210(__VA_ARGS__)


/* Choses one of EVAL_X/EVAL2_x macros depending on arguments count */
#define EVAL_SELECT(...) TOKEN_CAT_2(EVAL_, EVAL_ARGS_COUNT(__VA_ARGS__))
#define EVAL2_SELECT(...) TOKEN_CAT_2(EVAL2_, EVAL_ARGS_COUNT(__VA_ARGS__))

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
#define MAP(f, ...) EVAL_SELECT(__VA_ARGS__)(MAP_1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP_1)(f, peek, __VA_ARGS__)
#define MAP_1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP_0)(f, peek, __VA_ARGS__)

#define MAP2(f, ...) EVAL2_SELECT(__VA_ARGS__)(MAP2_1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP2_0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP2_1)(f, peek, __VA_ARGS__)
#define MAP2_1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP2_0)(f, peek, __VA_ARGS__)

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

#define MAP2_ARG(f, p, ...) EVAL2_SELECT(__VA_ARGS__)(MAP2_ARG1(f, p, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP2_ARG0(f, p, x, peek, ...) f(p, x) MAP_NEXT(peek, MAP2_ARG1)(f, p, peek, __VA_ARGS__)
#define MAP2_ARG1(f, p, x, peek, ...) f(p, x) MAP_NEXT(peek, MAP2_ARG0)(f, p, peek, __VA_ARGS__)

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
#define MAP_INDEX(f, ...) EVAL_SELECT(__VA_ARGS__)(MAP_INDEX1(f, (1), __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP_INDEX0(f, cnt, x, peek, ...) f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_NEXT(peek, MAP_INDEX1)(f, (EXPAND cnt, 1), peek, __VA_ARGS__)
#define MAP_INDEX1(f, cnt, x, peek, ...) f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_NEXT(peek, MAP_INDEX0)(f, (EXPAND cnt, 1), peek, __VA_ARGS__)

#define MAP2_INDEX(f, ...) EVAL2_SELECT(__VA_ARGS__)(MAP2_INDEX1(f, (1), __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP2_INDEX0(f, cnt, x, peek, ...) f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_NEXT(peek, MAP2_INDEX1)(f, (EXPAND cnt, 1), peek, __VA_ARGS__)
#define MAP2_INDEX1(f, cnt, x, peek, ...) f(ARGS_COUNT_ZERO(EXPAND cnt), x) MAP_NEXT(peek, MAP2_INDEX0)(f, (EXPAND cnt, 1), peek, __VA_ARGS__)


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
    MAP_SEP_ARG_IDX((,), init_array_element, a, 10, 20, 30) ;

    //this expands to:
    // a[0] = 10, a[1] = 20, a[2] = 30 ;

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

#define MAP2_LIST_ARG(f, p, ...) EVAL2_SELECT(__VA_ARGS__)(MAP2_LIST_ARG1(f, p, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define MAP2_LIST_ARG0(f, p, x, peek, ...) f(p, x) MAP_LIST_NEXT(peek, MAP2_LIST_ARG1)(f, p, peek, __VA_ARGS__)
#define MAP2_LIST_ARG1(f, p, x, peek, ...) f(p, x) MAP_LIST_NEXT(peek, MAP2_LIST_ARG0)(f, p, peek, __VA_ARGS__)


/* Experimental */

#define MAP_CHOOSE_END2() 0, 0
#define MAP_CHOOSE_END1(...) MAP_CHOOSE_END2
#define MAP_CHOOSE_END(...) MAP_CHOOSE_END1
#define MAP_CHOOSE0(test, t, f, ...) f MAP_OUT
#define MAP_CHOOSE1(test, t, f) MAP_CHOOSE0(test, t, f, 0)
#define MAP_CHOOSE_NEXT(test, t, f) MAP_CHOOSE1(MAP_CHOOSE_END test, t, f )


/* Apply macro f(prev, var) for each argument passed.
 * result of f() expansion is passed into next expansion of f() as 'prev' parameter along with next argument,
 * and it's result is passed into next f() until no more arguments left
 *
 * RECURSION(do_that, 100, 1)       ->                 do_that(100, 1)
 * RECURSION(do_that, 100, 1, 2)    ->         do_that(do_that(100, 1), 2)
 * RECURSION(do_that, 100, 1, 2, 3) -> do_that(do_that(do_that(100, 1), 2, 3)
 */
#define RECURSION(f, prev, ...) EVAL_SELECT(__VA_ARGS__)(RECURSION_0(f, prev, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define RECURSION_0(f, prev, x, peek, ...) MAP_CHOOSE_NEXT(peek, RECURSION_END, RECURSION_1)(f, f(prev, x), peek, __VA_ARGS__)
#define RECURSION_1(f, prev, x, peek, ...) MAP_CHOOSE_NEXT(peek, RECURSION_END, RECURSION_0)(f, f(prev, x), peek, __VA_ARGS__)
#define RECURSION_END(f, prev, ...) prev

/* Apply macro f(arg, prev, var) for each argument passed.
 * same as RECUSRION(), but with an argument.
 *
 * RECURSION_ARG(do_that, 9, 100, 1)       ->                       do_that(9, 100, 1)
 * RECURSION_ARG(do_that, 9, 100, 1, 2)    ->            do_that(9, do_that(9, 100, 1), 2)
 * RECURSION_ARG(do_that, 9, 100, 1, 2, 3) -> do_that(9, do_that(9, do_that(9, 100, 1), 2, 3)
 */
#define RECURSION_ARG(f, arg, prev, ...) EVAL_SELECT(__VA_ARGS__)(RECURSION_ARG0(f, arg, prev, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
#define RECURSION_ARG0(f, arg, prev, x, peek, ...) MAP_CHOOSE_NEXT(peek, RECURSION_ARG_END, RECURSION_ARG1)(f, arg, f(arg, prev, x), peek, __VA_ARGS__)
#define RECURSION_ARG1(f, arg, prev, x, peek, ...) MAP_CHOOSE_NEXT(peek, RECURSION_ARG_END, RECURSION_ARG0)(f, arg, f(arg, prev, x), peek, __VA_ARGS__)
#define RECURSION_ARG_END(f, arg, prev, ...) prev


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

#endif // POOR_MAP_H
