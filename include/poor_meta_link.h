// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_META_LINK_H
#define POOR_META_LINK_H
#include <poor_traits.h>

/* This is totally experimental */

/* Meta link is a way to embed a reference to a specific function, object or constant inside a type,
 * so you can use that type later to get it back.
 *
 * 	int do_stuff(int a) { return a * 2; }
 * 	poor_meta_link_define(magic_type, do_stuff);
 *
 * 	auto fn = poor_meta_link_get(magic_type);
 * 	fn(5);                             // calls do_stuff(5)
 * 	poor_meta_link_get(magic_type)(5); // the same
 *
 * 	static struct config cfg;
 * 	poor_meta_link_define(config_type, &cfg);
 * 	poor_meta_link_get(config_type)->verbosity = 2; // changes cfg
 *
 * 	poor_meta_link_define(answer_type, 42);
 * 	int answer = poor_meta_link_get(answer_type);   // 42
 *
 * The value must be valid in a static initializer: a function, the address of an object with static storage
 * duration (extern ones too), a string literal or a constant. Functions and objects are linked by address,
 * constants are copied. Local and thread_local variables don't work.
 * Arrays decay to a pointer to their first element, link &array to keep the size.
 *
 * Instead of the type you can pass any expression of it, it is never evaluated.
 * Objects of the type take one byte, so in a union with other data they cost nothing.
 *
 * Limits:
 * - Each define takes a slot from __COUNTER__ (C2y), and other users of __COUNTER__ use slots up too.
 *   There are POOR_META_LINK_SLOTS slots per translation unit: 100 by default, or 200, 300, ... 1000
 *   if you define it before including this header. More slots make every call and the header bigger.
 *   Running out fails with "out of slots".
 * - Use get in static inline or ordinary functions only: the slots are static, which plain inline
 *   functions may not use before C2y. GCC warns once per slot.
 * - get is not a constant expression, so it can't be used in static_assert or as an array size.
 */

#ifndef POOR_META_LINK_SLOTS
# define POOR_META_LINK_SLOTS 100
#endif

#if POOR_META_LINK_SLOTS % 100 || POOR_META_LINK_SLOTS < 100 || POOR_META_LINK_SLOTS > 1000
# error "POOR_META_LINK_SLOTS must be 100, 200, ... or 1000"
#endif

#define poor_meta_link_define(name, value) h_meta_link_counter_push h_meta_link_define(name, value, __COUNTER__)

#define poor_meta_link_get(key) ((void)0, _Generic((typeof_unqual(key) *)nullptr h_meta_link_slots(h_meta_link_of))->h_value)

/* __COUNTER__ is C2y, Clang's -pedantic flags it in C23 */
#ifdef __clang__
# define h_meta_link_counter_push _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wc2y-extensions\"")
# define h_meta_link_counter_pop _Pragma("clang diagnostic pop")
#else
# define h_meta_link_counter_push
# define h_meta_link_counter_pop
#endif

/* At block scope the slot shadows the file scope slot */
#define h_meta_link_define(name, value, n) h_meta_link_define_n(name, value, n)
#define h_meta_link_define_n(name, value, n) h_meta_link_counter_pop					\
	static_assert(n < POOR_META_LINK_SLOTS, "poor_meta_link_define: out of slots");			\
	struct h_meta_link_type_##n { typeof((void)0, (value)) h_value; };				\
	static const struct h_meta_link_type_##n h_meta_link_obj_##n = {value};				\
	_Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wshadow\"")			\
	[[maybe_unused]] static const struct h_meta_link_type_##n *const h_meta_link_##n = &h_meta_link_obj_##n;	\
	_Pragma("GCC diagnostic pop")									\
	typedef h_meta_link_key_##n name

#define h_meta_link_of(n) , h_meta_link_key_##n *: h_meta_link_##n

#define h_meta_link_cat(a, b) h_meta_link_primitive_cat(a, b)
#define h_meta_link_primitive_cat(a, b) a##b

#define h_meta_link_slots(m) h_meta_link_cat(h_meta_link_slots_, POOR_META_LINK_SLOTS)(m)
#define h_meta_link_slots_100(m) h_meta_link_tens(m, ) h_meta_link_tens(m, 1) h_meta_link_tens(m, 2)	\
	h_meta_link_tens(m, 3) h_meta_link_tens(m, 4) h_meta_link_tens(m, 5) h_meta_link_tens(m, 6)		\
	h_meta_link_tens(m, 7) h_meta_link_tens(m, 8) h_meta_link_tens(m, 9)
#define h_meta_link_slots_200(m) h_meta_link_slots_100(m) h_meta_link_hundred(m, 1)
#define h_meta_link_slots_300(m) h_meta_link_slots_200(m) h_meta_link_hundred(m, 2)
#define h_meta_link_slots_400(m) h_meta_link_slots_300(m) h_meta_link_hundred(m, 3)
#define h_meta_link_slots_500(m) h_meta_link_slots_400(m) h_meta_link_hundred(m, 4)
#define h_meta_link_slots_600(m) h_meta_link_slots_500(m) h_meta_link_hundred(m, 5)
#define h_meta_link_slots_700(m) h_meta_link_slots_600(m) h_meta_link_hundred(m, 6)
#define h_meta_link_slots_800(m) h_meta_link_slots_700(m) h_meta_link_hundred(m, 7)
#define h_meta_link_slots_900(m) h_meta_link_slots_800(m) h_meta_link_hundred(m, 8)
#define h_meta_link_slots_1000(m) h_meta_link_slots_900(m) h_meta_link_hundred(m, 9)
#define h_meta_link_hundred(m, h) h_meta_link_tens(m, h##0) h_meta_link_tens(m, h##1) h_meta_link_tens(m, h##2)	\
	h_meta_link_tens(m, h##3) h_meta_link_tens(m, h##4) h_meta_link_tens(m, h##5) h_meta_link_tens(m, h##6)	\
	h_meta_link_tens(m, h##7) h_meta_link_tens(m, h##8) h_meta_link_tens(m, h##9)
#define h_meta_link_tens(m, t) m(t##0) m(t##1) m(t##2) m(t##3) m(t##4) m(t##5) m(t##6) m(t##7) m(t##8) m(t##9)

/* Structs are without tags, so they are compatible even if slots will be different */
#define h_meta_link_slot(n) typedef struct { unsigned char h_key; } h_meta_link_key_##n; struct h_meta_link_type_##n;	\
	[[maybe_unused]] static const struct h_meta_link_type_##n *const h_meta_link_##n;
h_meta_link_slots(h_meta_link_slot)

#endif // POOR_META_LINK_H
