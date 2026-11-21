// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_ASAN_H
#define POOR_ASAN_H
#include <stddef.h>

/* Memory poisoning for AddressSanitizer, without #ifdefs in your code.
 *
 * Poisoned memory must not be touched: AddressSanitizer stops the program with a "use-after-poison"
 * report at the first read or write of it. Pools and allocators poison the memory they own but
 * have not handed out, so that using it is caught like using freed memory.
 *
 * Without AddressSanitizer (-fsanitize=address) nothing here does anything,
 * but the arguments are checked and evaluated the same way, so code builds the same in both cases.
 *
 * All macro arguments are only evaluated once.
 *
 * AddressSanitizer tracks memory in blocks of 8 bytes, where it can only poison the end of a block.
 * So align what you poison to 8 bytes, otherwise the last bytes of it may stay accessible.
 *
 * Poison stays until you remove it: unpoison memory before you give it back to its owner.
 *
 * Quick example:
 *
 * 	struct job {
 * 		long long id;
 * 	};
 *
 * 	static struct job pool[16];
 *
 * 	poor_asan_poison_object(&pool);       // nobody has a job yet
 *
 * 	struct job *j = &pool[3];
 * 	poor_asan_unpoison_object(j);         // hand a job out
 * 	j->id = 1;
 *
 * 	poor_asan_poison_object(j);           // take it back
 * 	println(poor_asan_is_poisoned(j));    // prints: true (false without AddressSanitizer)
 * 	j->id = 2;                            // AddressSanitizer: use-after-poison
 */

/* Poisons 'bytes' bytes of memory starting at 'addr'. */
#define poor_asan_poison_region(addr, bytes) h_asan_poison(addr, bytes)

/* Makes 'bytes' bytes of memory starting at 'addr' accessible again. */
#define poor_asan_unpoison_region(addr, bytes) h_asan_unpoison(addr, bytes)

/* Returns true if the byte at 'addr' is poisoned, by you or by AddressSanitizer itself (freed memory,
 * red zones), false otherwise. Without AddressSanitizer it is always false. */
#define poor_asan_is_poisoned(addr) h_asan_is_poisoned(addr)

/* The same for a single object. These are statements. */

/* Poisons the object 'ref' points to. The size comes from its type, so a pointer to an array poisons
 * the whole array, and a pointer to void is rejected. */
#define poor_asan_poison_object(ref) h_asan_object(h_asan_poison, ref, #ref)

/* Makes the object 'ref' points to accessible again. */
#define poor_asan_unpoison_object(ref) h_asan_object(h_asan_unpoison, ref, #ref)

/****** ------------------------------------------------------------------------------ *******/
/*** Implementation macros. Everything below should not be used and can be changed anytime ***/
/****** ------------------------------------------------------------------------------ *******/

#define h_asan_object(fn, ref, text) do {									\
	auto const _tmp_asan_ref_ = (ref);									\
	static_assert(_Generic(_tmp_asan_ref_, void *: 0, const void *: 0, volatile void *: 0,			\
			       const volatile void *: 0, default: 1),						\
		      "object (" text ") must not be a pointer to void");					\
	fn(_tmp_asan_ref_, sizeof(*_tmp_asan_ref_));								\
} while(0)

#if defined(__SANITIZE_ADDRESS__)
#define h_asan_enabled 1
#elif defined(__has_feature)
#define h_asan_enabled __has_feature(address_sanitizer)
#else
#define h_asan_enabled 0
#endif

#if h_asan_enabled

/* The runtime is declared here to not depend on <sanitizer/asan_interface.h>, which may be included too */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
void __asan_poison_memory_region(const volatile void *addr, size_t size);
void __asan_unpoison_memory_region(const volatile void *addr, size_t size);
int __asan_address_is_poisoned(const volatile void *addr);
#pragma GCC diagnostic pop

static inline void h_asan_poison(const volatile void *addr, size_t bytes) {
	__asan_poison_memory_region(addr, bytes);
}

static inline void h_asan_unpoison(const volatile void *addr, size_t bytes) {
	__asan_unpoison_memory_region(addr, bytes);
}

static inline bool h_asan_is_poisoned(const volatile void *addr) {
	return __asan_address_is_poisoned(addr);
}

#else

static inline void h_asan_poison(const volatile void *, size_t) {
}

static inline void h_asan_unpoison(const volatile void *, size_t) {
}

static inline bool h_asan_is_poisoned(const volatile void *) {
	return false;
}

#endif
#undef h_asan_enabled

#endif // POOR_ASAN_H
