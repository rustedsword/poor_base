// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_VECTOR_H
#define POOR_VECTOR_H
#include <poor_array.h>

/* A vector with an attached buffer.
 *
 * The vector stores only an integer that counts used items.
 * To give the vector storage you need to attach a buffer at compile time.
 * The buffer and the vector must be inside the same struct, so offsetof can be used to locate it.
 *
 * No bounds checks are performed. Check them yourself when needed.
 *
 * All macro arguments are only evaluated once.
 *
 * If you need to copy a vector you can do that only by copying the enclosing struct together
 * with the attached buffer.
 *
 * GCC's _FORTIFY_SOURCE can falsely abort string functions that write to the buffer of a struct
 * picked at runtime from an array. To avoid that, put the buffer after the vector, and get pointers
 * to items with poor_vector_at() instead of adding to a pointer you already have.
 *
 * Quick example:
 *
 * 	struct job {
 * 		int id;
 * 	};
 *
 * 	poor_vector_declare(job_vector, uint8_t); // counts jobs in a uint8_t
 *
 * 	struct worker {
 * 		job_vector jobs;
 * 		struct job queue[16]; // room for 16 jobs
 * 	};
 *
 * 	poor_vector_attach_buffer(job_vector, struct worker, jobs, queue);
 *
 * 	struct worker w = {};
 *
 * 	poor_vector_append(&w.jobs, (struct job){.id = 1});    // vector is now: [1]
 * 	poor_vector_append(&w.jobs, (struct job){.id = 2});    // vector is now: [1, 2]
 * 	poor_vector_insert(&w.jobs, 0, (struct job){.id = 3}); // vector is now: [3, 1, 2]
 *
 * 	// Loop through everything:
 * 	poor_vector_foreach(&w.jobs, j)
 * 		println("job ", j->id);
 * 	// prints:
 * 	// job 3
 * 	// job 1
 * 	// job 2
 *
 * 	// Remove the second item (which is 1):
 * 	poor_vector_remove(&w.jobs, 1);
 * 	println(poor_vector_size(&w.jobs)); // prints: 2
 *
 * 	// Add only if there is room:
 * 	if(!poor_vector_full(&w.jobs))
 * 		poor_vector_append(&w.jobs, (struct job){.id = 4});
 *
 * 	// Removing items inside a loop? Loop backward!
 * 	poor_vector_foreach_bw(&w.jobs, j)
 * 		poor_vector_remove_ref(&w.jobs, j);
 *
 * 	println(poor_vector_empty(&w.jobs)); // prints: true
 */

/* Declares vector type 'name' that counts its items in 'size_type', which can be any unqualified integer type.
 * 'size_type' is optional and defaults to unsigned.
 * The vector has the size and alignment of a pointer, or of 'size_type' if they are bigger. */
#define poor_vector_declare(name, ...) POOR_EAT_ONE_ARG(__VA_ARGS__ ## __VA_OPT__())	\
	h_vector_declare_as(name, POOR_TAKE_FIRST_ARG(__VA_OPT__(__VA_ARGS__,) unsigned))

/* Attaches the buffer to the vector in container_of style.
 *
 * 	poor_vector_declare(crappy_ints);
 * 	struct some_crap {
 * 		int int_buffer[10];
 * 		...
 * 		crappy_ints vec;
 * 		...
 * 	};
 * 	poor_vector_attach_buffer(crappy_ints, struct some_crap, vec, int_buffer);
 */
#define poor_vector_attach_buffer(name, struct_type, vec_member, buf_member) h_vector_attach_buffer(name,	\
	h_vector_cat(h_vector_buffer_, name), h_vector_cat(h_vector_meta_, name), struct_type, vec_member, buf_member)

/* Returns the number of items in the vector. */
#define poor_vector_size(vec) ((void)0, (vec)->size)

/* Sets the number of items in the vector. */
#define poor_vector_set_size(vec, size) h_vector_set_size(vec, #vec, size)

/* The number of items the buffer holds, as an integer constant expression. */
#define poor_vector_capacity(vec) h_vector_capacity(vec)

/* Returns true if the vector has no items, false otherwise. */
#define poor_vector_empty(vec) ((bool)!(vec)->size)

/* Returns true if there is no room for another item, false otherwise. */
#define poor_vector_full(vec) ((bool)((vec)->size == poor_vector_capacity(vec)))

/* Returns a pointer to the whole buffer as an array, including the unused items. */
#define poor_vector_buffer(vec) h_vector_buffer(vec)

/* Returns a typed pointer to the item at 'index', counting from 0. */
#define poor_vector_at(vec, index) h_vector_at(vec, index)

/* Adding and removing items. These are statements. */

/* Add 'value' to the end. 'value' is assigned straight into the buffer, without a temporary copy. */
#define poor_vector_append(vec, ...) h_vector_append(vec, #vec, __VA_ARGS__)

/* Insert 'value' at 'index', moving the items from there one place up.
 * 'index' must not be greater than the current vector size.
 * 'value' is assigned straight into the buffer at the selected 'index', without a temporary copy,
 * but after the items moved up. */
#define poor_vector_insert(vec, index, ...) h_vector_insert(vec, #vec, index, __VA_ARGS__)

/* Remove the item at 'index', moving the items after it one place down. */
#define poor_vector_remove(vec, index) h_vector_remove(vec, #vec, index)

/* Remove the item 'ref' points to, moving the items after it one place down. */
#define poor_vector_remove_ref(vec, ref) h_vector_remove_ref(vec, #vec, ref, #ref)

/* Remove all items. */
#define poor_vector_clear(vec) ((void)(h_vector_mut(vec, #vec)->size = 0))

/* Loops: */

/* Loop forward from first to last item.
 * 'ref' is declared automatically with your item's pointer type.
 * Warning: do NOT add or remove items inside this loop! */
#define poor_vector_foreach(vec, ref) h_vector_foreach(vec, ref, h_vector_cat(h_vector_of_, ref))

/* Loop backward from last to first item. Removing 'ref' is fine here. */
#define poor_vector_foreach_bw(vec, ref) h_vector_foreach_bw(vec, ref, h_vector_cat(h_vector_of_, ref))

/****** ------------------------------------------------------------------------------ *******/
/*** Implementation macros. Everything below should not be used and can be changed anytime ***/
/****** ------------------------------------------------------------------------------ *******/

#define h_vector_cat(a, ...) h_vector_primitive_cat(a, __VA_ARGS__)
#define h_vector_primitive_cat(a, ...) a ## __VA_ARGS__

#define h_vector_set_size(vec, text, n) ((void)(h_vector_mut(vec, text)->size = (n)))

#define h_vector_append(vec, text, ...) do {							\
	h_vector_capture_mut(_tmp_vector_, vec, text);						\
	*h_vector_end(_tmp_vector_) = (__VA_ARGS__);						\
	_tmp_vector_->size += 1;								\
} while(0)

#define h_vector_insert(vec, text, index, ...) do {						\
	h_vector_capture_mut(_tmp_vector_, vec, text);						\
	auto const _tmp_vector_at_ = h_vector_begin(_tmp_vector_) + (index);			\
	h_vector_move(_tmp_vector_at_ + 1, _tmp_vector_at_, h_vector_end(_tmp_vector_));	\
	*_tmp_vector_at_ = (__VA_ARGS__);							\
	_tmp_vector_->size += 1;								\
} while(0)

#define h_vector_remove(vec, text, index) do {							\
	h_vector_capture_mut(_tmp_vector_, vec, text);						\
	auto const _tmp_vector_at_ = h_vector_begin(_tmp_vector_) + (index);			\
	h_vector_move(_tmp_vector_at_, _tmp_vector_at_ + 1, h_vector_end(_tmp_vector_));	\
	_tmp_vector_->size -= 1;								\
} while(0)

#define h_vector_remove_ref(vec, text, ref, ref_text) do {								\
	h_vector_capture_mut(_tmp_vector_, vec, text);									\
	auto const _tmp_vector_ref_ = (ref);										\
	static_assert(!is_pointer_to_const((typeof_unqual(_tmp_vector_ref_))nullptr), "item (" ref_text ") is const");	\
	static_assert(_Generic((typeof_unqual(_tmp_vector_ref_))nullptr, h_vector_item(_tmp_vector_) *: 1,		\
			       const h_vector_item(_tmp_vector_) *: 1, default: 0),					\
		      "item (" ref_text ") must point to an item of the vector");					\
	h_vector_move(_tmp_vector_ref_, _tmp_vector_ref_ + 1, h_vector_end(_tmp_vector_));				\
	_tmp_vector_->size -= 1;											\
} while(0)

/* The vector is also the flag of the outer loop, which runs once */
#define h_vector_foreach(vec, ref, of) for(auto of = (vec); of; of = nullptr)				\
	for(h_vector_ref_type(of) ref = h_vector_begin(of); ref < h_vector_end(of); ref++)

/* The end is checked too, in case the loop removed more than 'ref' */
#define h_vector_foreach_bw(vec, ref, of) for(auto of = (vec); of; of = nullptr)				\
	for(h_vector_ref_type(of) ref = h_vector_end(of); ref > h_vector_begin(of) && ref <= h_vector_end(of) && (--ref, true);)

#define h_vector_capacity(vec) UNSAFE_ARRAY_SIZE(*(vec)->h_meta->h_buffer)
/* The buffer is taken as the only item of an array, so GCC bounds it by the struct that holds it.
 * With a plain cast, GCC bounds it by the vector it was computed from, and fortified string functions abort.
 * Items are indexed for the same reason: GCC turns pointer additions into offsets from the vector */
#define h_vector_buffer(vec) (&(*(h_vector_view_type(vec))h_vector_data(vec))[0])
#define h_vector_at(vec, index) (&(*h_vector_buffer(vec))[index])
#define h_vector_begin(vec) h_vector_at(vec, 0)
#define h_vector_end(vec) (h_vector_begin(vec) + (vec)->size)
#define h_vector_data(vec) h_vector_fn(vec, h_vector_data)(vec,						\
	sizeof((vec)->h_meta->h_vector_offset) - 1, sizeof((vec)->h_meta->h_buffer_offset) - 1)

/* Only integer types turn 0.5 into an integer constant expression */
#define h_vector_is_integer(type) is_const_expr((typeof_unqual(type))0.5)

/* Expands the size type before h_vector_declare() stringifies it, so errors show the type itself */
#define h_vector_declare_as(name, size_type) \
	h_vector_declare(name, h_vector_cat(h_vector_tag_, name), h_vector_cat(h_vector_meta_, name), size_type)

/* Declaring the meta tag first binds h_meta to this scope's meta, not to a shadowed outer vector's one */
#define h_vector_declare(name, tag, meta, size_type)								\
	struct meta;												\
	typedef union tag {											\
		size_type size;											\
		struct meta *h_meta;										\
		static_assert(h_vector_is_integer(size_type) &&						\
			      _Generic((size_type *)nullptr, typeof_unqual(size_type) *: 1, default: 0),		\
			      "vector size (" #size_type ") must be an unqualified integer type");		\
	} name

/* The meta struct is never instantiated. Its members only record, in their types,
 * where the vector and the buffer are in the struct and what the buffer holds.
 * The buffer typedef forces a compile error if a vector is attached to buffers of conflicting types
 * (which Clang's duplicate struct check misses) */
#define h_vector_attach_buffer(name, buffer, meta, type, path, member)						\
	typedef typeof(((type *)nullptr)->member) buffer;							\
	struct meta {												\
		unsigned char h_vector_offset[offsetof(type, path) + 1];					\
		unsigned char h_buffer_offset[offsetof(type, member) + 1];					\
		buffer *h_buffer;										\
		static_assert(_Generic(&((type *)nullptr)->path, name *: 1, default: 0),			\
			      "vector (" #path ") must be an unqualified " #name);				\
		static_assert(_Generic(&((type *)nullptr)->member,						\
				       typeof((void)0, ((type *)nullptr)->member[0]) (*)[]: 1, default: 0),	\
			      "buffer (" #member ") must be a one-dimensional array of unqualified items");	\
		static_assert(h_vector_counts(name, UNSAFE_ARRAY_SIZE(((type *)nullptr)->member)),		\
			      "buffer (" #member ") has more items than " #name " can count");			\
	}

#define h_vector_counts(name, n) ((unsigned long long)(typeof(((name *)nullptr)->size))(n) == (n))

#define h_vector_item(vec) typeof_unqual(**(vec)->h_meta->h_buffer)
#define h_vector_ref_type(vec) typeof(qualify_type_as(h_vector_item(vec), (vec)->size)) *
#define h_vector_view_type(vec) typeof(qualify_type_as(h_vector_item(vec), (vec)->size)) (*)[1][h_vector_capacity(vec)]
#define h_vector_mut(vec, text) \
	(static_assert_expr(!is_pointer_to_const((typeof(&(vec)->size))nullptr), "vector (" text ") is const"), (vec))
#define h_vector_capture_mut(name, vec, text) auto const name = (vec);					\
	static_assert(!is_pointer_to_const((typeof(&name->size))nullptr), "vector (" text ") is const")
#define h_vector_fn(vec, fn) _Generic((typeof(&(vec)->size))nullptr,						\
	typeof_unqual((vec)->size) *: fn##_mut, const typeof_unqual((vec)->size) *: fn##_const)

static inline void *h_vector_data_mut(void *vec, size_t vector_offset, size_t buffer_offset) {
	return (unsigned char *)vec - vector_offset + buffer_offset;
}

static inline const void *h_vector_data_const(const void *vec, size_t vector_offset, size_t buffer_offset) {
	return (const unsigned char *)vec - vector_offset + buffer_offset;
}

/* Moves the items from 'from' up to 'end' to 'to'. The distance between them is never negative,
 * so -Wsign-conversion is wrong about converting it to size_t */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
static inline void h_vector_move(void *to, const void *from, const void *end) {
	memmove(to, from, (const unsigned char *)end - (const unsigned char *)from);
}
#pragma GCC diagnostic pop

#endif // POOR_VECTOR_H
