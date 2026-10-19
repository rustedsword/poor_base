// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_META_NUMBER_H
#define POOR_META_NUMBER_H
#include <poor_traits.h>

/* Meta number is a way to embed a number inside a type, so you can get it back from that type later
 * as an integer constant expression.
 *
 * 	typedef union {
 * 		char c;
 * 		poor_meta_number(1000);
 * 	} small;
 *
 * 	static_assert(sizeof(small) == 1);
 * 	static_assert(poor_meta_number_get(small) == 1000);
 *
 * 	typedef poor_meta_number_8(42) answer;
 * 	int numbers[poor_meta_number_8_get(answer)];
 *
 * poor_meta_number() takes an integer constant expression not wider than unsigned long long,
 * negative values come back wrapped. poor_meta_number_32(), _16() and _8() take values from 0 to
 * 2^32 - 1, 2^16 - 1 and 2^8 - 1. Their get reads 11, 6 and 3 members instead of 22, so it compiles faster.
 * Each get works only with its own variant and returns unsigned long long.
 * Instead of the type you can pass any expression of it, it is never evaluated.
 *
 * Objects of the type take one byte, so in a union with other data they cost nothing.
 * Each poor_meta_number() is a new type, typedef it to share one.
 */

#define poor_meta_number(value) h_meta_number(64, value)
#define poor_meta_number_32(value) h_meta_number(32, value)
#define poor_meta_number_16(value) h_meta_number(16, value)
#define poor_meta_number_8(value) h_meta_number(8, value)

#define poor_meta_number_get(key) h_meta_number_get(64, key)
#define poor_meta_number_32_get(key) h_meta_number_get(32, key)
#define poor_meta_number_16_get(key) h_meta_number_get(16, key)
#define poor_meta_number_8_get(key) h_meta_number_get(8, key)

#define h_meta_number(bits, value) union {									\
	static_assert(_Generic((value) + 0ull, unsigned long long: 1, default: 0),			\
		      "poor_meta_number: value must be an integer not wider than unsigned long long");	\
	static_assert((unsigned long long)(value) >> (bits - 1) >> 1 == 0,				\
		      "poor_meta_number: value must be from 0 to 2^" #bits " - 1");			\
	h_meta_number_each_##bits(h_meta_number_digit, bits, value) }

#define h_meta_number_get(bits, key) (0 h_meta_number_each_##bits(h_meta_number_read, bits, typeof_unqual(key)))

/* A byte-sized member holds 3 bits: width 8 - d stores digit d and 217 / (2^width - 1 + 30) reads it back */
#define h_meta_number_digit(bits, value, n) unsigned _BitInt(8 - ((unsigned long long)(value) >> 3 * n & 7)) h_digit##bits##_##n;
#define h_meta_number_read(bits, type, n) | 217 / ((typeof(((type *)0)->h_digit##bits##_##n))-1 + 30ull) << 3 * n

#define h_meta_number_each_8(m, a, b) m(a, b, 0) m(a, b, 1) m(a, b, 2)
#define h_meta_number_each_16(m, a, b) h_meta_number_each_8(m, a, b) m(a, b, 3) m(a, b, 4) m(a, b, 5)
#define h_meta_number_each_32(m, a, b) h_meta_number_each_16(m, a, b) m(a, b, 6) m(a, b, 7) m(a, b, 8) m(a, b, 9) m(a, b, 10)
#define h_meta_number_each_64(m, a, b) h_meta_number_each_32(m, a, b) m(a, b, 11) m(a, b, 12) m(a, b, 13) m(a, b, 14)	\
	m(a, b, 15) m(a, b, 16) m(a, b, 17) m(a, b, 18) m(a, b, 19) m(a, b, 20) m(a, b, 21)

#endif // POOR_META_NUMBER_H
