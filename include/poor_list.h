// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_LIST_H
#define POOR_LIST_H
#include <poor_traits.h>

/* A doubly linked list for normal human beings who hate debugging pointers.
 *
 * What is an "intrusive" list?
 * Normally, linked lists allocate little wrapper nodes on the heap (malloc) for every item.
 * That's slow and fragments memory.
 * Here, you just put a `struct poor_list_node link;` right inside your own struct.
 * No extra allocations, cache-friendly, and very fast.
 *
 * Why is this better than typical C list hacks (like Linux kernel's list_head)?
 * It's actually type-safe! When you create a list type with `poor_list_define(job_list, struct job, link)`,
 * the list remembers what struct it holds. If you accidentally try to put a `struct cat`
 * into your `job_list`, the compiler will yell at you instead of silently corrupting memory.
 * Also, if your list is const, it gives you back const pointers.
 *
 * IMPORTANT RULES (please read so you don't shoot yourself in the foot):
 *
 * 1. DO NOT memset() or zero-initialize this list!
 *    This is a circular list: an empty list's pointers point to itself (head.next == &head).
 *    If you memset it to 0, it will crash the moment you touch it.
 *    Always initialize it with POOR_LIST_INIT(my_list) or poor_list_init(&my_list).
 *
 * 2. DO NOT copy or pass the list by value!
 *    Because the head points to its own address, copying it (list2 = list1 or memcpy)
 *    means list2 still points to list1's memory address. Always pass it around by pointer (&my_list).
 *
 * 3. Safe macros:
 *    Except for POOR_LIST_INIT(), all macro arguments are only evaluated once,
 *    so calling poor_list_append(list, get_next_job()) won't call get_next_job() twice.
 *
 * Quick example:
 *
 * 	struct job {
 * 		int id;
 * 		struct poor_list_node link; // embed the list hook here
 * 	};
 *
 * 	poor_list_define(job_list, struct job, link);
 *
 * 	job_list jobs = POOR_LIST_INIT(jobs); // MUST pass variable name itself!
 * 	struct job a = {.id = 1}, b = {.id = 2}, c = {.id = 3};
 *
 * 	poor_list_append(&jobs, &a);   // list is now: [1]
 * 	poor_list_append(&jobs, &b);   // list is now: [1, 2]
 * 	poor_list_prepend(&jobs, &c);  // list is now: [3, 1, 2]
 *
 * 	// Loop through everything:
 * 	poor_list_foreach(&jobs, j)
 * 		println("job ", j->id);
 * 	// prints:
 * 	// job 3
 * 	// job 1
 * 	// job 2
 *
 * 	// Remove the first item (which is 'c'):
 * 	poor_list_remove(&jobs, poor_list_first(&jobs));
 * 	println(poor_list_length(&jobs)); // prints: 2
 *
 * 	// Deleting stuff inside a loop? Use foreach_safe!
 * 	// Regular foreach will break if you remove the current item.
 * 	poor_list_foreach_safe(&jobs, j)
 * 		poor_list_remove(&jobs, j);
 *
 * 	println(poor_list_empty(&jobs)); // prints: true
 */

/* The link node you embed inside your struct.
 * Just two pointers: prev and next. That's the only overhead. */
struct poor_list_node {
	struct poor_list_node *prev, *next;
};

/* Defines list type 'name' for your struct 'type', using 'member' as its link.
 *
 * Your struct must be fully defined, and 'member' must be a `struct poor_list_node`.
 * Works whether or not you called poor_list_declare(name) first. */
#define poor_list_define(name, type, member) \
	poor_list_declare(name); h_list_define(h_list_cat(h_list_item_, name), h_list_cat(h_list_meta_, name), type, member)

/* Forward-declares list type 'name' before its struct is complete.
 * Use this when a struct needs to hold a list of its own type (like folders with subfolders):
 *
 *     poor_list_declare(folder_list);
 *     struct folder {
 *         char name[64];
 *         struct poor_list_node link;
 *         folder_list subfolders;
 *     };
 *     poor_list_define(folder_list, struct folder, link);
 *
 * You can embed or pass a declared list around immediately (e.g. in headers),
 * but you must call poor_list_define() once the struct is complete to access its items. */
#define poor_list_declare(name) h_list_declare(name, h_list_cat(h_list_head_, name), h_list_cat(h_list_meta_, name))

/* Static initializer for a list variable.
 * Usage: my_list_t my_list = POOR_LIST_INIT(my_list);
 * Must pass the variable's own name so its pointers can point to itself. */
#define POOR_LIST_INIT(name) { .head = { &(name).head, &(name).head } }

/* Runtime initializer. Call this to initialize or reset a list.
 * Usage: poor_list_init(&my_list); */
#define poor_list_init(list) h_list_init(h_list_mut_head(list))

/* Returns true if the list has zero items, false otherwise. */
#define poor_list_empty(list) ((bool)!poor_list_first(list))

/* Returns the total number of items in the list.
 * Note: this walks through the list (O(N)), so avoid calling it repeatedly in hot loops. */
#define poor_list_length(list) h_list_length(&(list)->head)

/* Navigation: return a typed pointer to your struct, or nullptr when empty or at the end. */

/* Get the first item in the list, or nullptr if empty. */
#define poor_list_first(list) h_list_ref(list, h_list_first(&(list)->head, h_list_offset(list)))

/* Get the last item in the list, or nullptr if empty. */
#define poor_list_last(list)  h_list_ref(list, h_list_last(&(list)->head, h_list_offset(list)))

/* Get the item after 'ref', or nullptr if 'ref' is the last item. */
#define poor_list_next(list, ref) h_list_ref(list, h_list_entry(&(list)->head, h_list_node(list, ref)->next, h_list_offset(list)))

/* Get the item before 'ref', or nullptr if 'ref' is the first item. */
#define poor_list_prev(list, ref) h_list_ref(list, h_list_entry(&(list)->head, h_list_node(list, ref)->prev, h_list_offset(list)))

/* Adding and removing items: */

/* Add 'ref' to the end (tail) of the list. */
#define poor_list_append(list, ref)  h_list_insert(h_list_mut_head(list)->prev, h_list_mut_node(list, ref))

/* Add 'ref' to the beginning (head) of the list. */
#define poor_list_prepend(list, ref) h_list_insert(h_list_mut_head(list), h_list_mut_node(list, ref))

/* Insert 'ref' right before existing item 'at'. */
#define poor_list_insert_before(list, at, ref) \
	((void)h_list_mut_head(list), h_list_insert(h_list_mut_node(list, at)->prev, h_list_mut_node(list, ref)))

/* Insert 'ref' right after existing item 'at'. */
#define poor_list_insert_after(list, at, ref) \
	((void)h_list_mut_head(list), h_list_insert(h_list_mut_node(list, at), h_list_mut_node(list, ref)))

/* Unlink 'ref' from the list. Its link pointers remain stale. */
#define poor_list_remove(list, ref) ((void)h_list_mut_head(list), h_list_remove(h_list_mut_node(list, ref)))

/* Loops: */

/* Loop forward from first to last item.
 * 'ref' is declared automatically with your struct's pointer type.
 * Warning: do NOT remove 'ref' from the list inside this loop! */
#define poor_list_foreach(list, ref) \
	h_list_foreach(list, ref, h_list_cat(h_list_of_, ref), h_list_cat(h_once_, ref))

/* Loop backward from last to first item. Do NOT remove 'ref' inside this loop! */
#define poor_list_foreach_bw(list, ref) \
	h_list_foreach_bw(list, ref, h_list_cat(h_list_of_, ref), h_list_cat(h_once_, ref))

/* Loop forward, safe against removing 'ref'.
 * Use this if you plan to call poor_list_remove(list, ref) during the loop. */
#define poor_list_foreach_safe(list, ref) \
	h_list_foreach_safe(list, ref, h_list_cat(h_list_of_, ref), h_list_cat(h_once_, ref), h_list_cat(h_next_, ref))

/* Loop backward, safe against removing 'ref'. */
#define poor_list_foreach_bw_safe(list, ref) \
	h_list_foreach_bw_safe(list, ref, h_list_cat(h_list_of_, ref), h_list_cat(h_once_, ref), h_list_cat(h_next_, ref))

/****** ------------------------------------------------------------------------------ *******/
/*** Implementation macros. Everything below should not be used and can be changed anytime ***/
/****** ------------------------------------------------------------------------------ *******/

#define h_list_cat(a, ...) h_list_primitive_cat(a, __VA_ARGS__)
#define h_list_primitive_cat(a, ...) a ## __VA_ARGS__

#define h_list_capture(list, of, once) for(typeof_unqual(list) of = (list), *once = &of; once; once = nullptr)

#define h_list_foreach(list, ref, of, once) h_list_capture(list, of, once) \
	for(h_list_ref_type(list) ref = poor_list_first(of); ref; ref = poor_list_next(of, ref))

#define h_list_foreach_bw(list, ref, of, once) h_list_capture(list, of, once) \
	for(h_list_ref_type(list) ref = poor_list_last(of); ref; ref = poor_list_prev(of, ref))

#define h_list_foreach_safe(list, ref, of, once, next) h_list_capture(list, of, once) \
	for(h_list_ref_type(list) ref = poor_list_first(of), next; ref && (next = poor_list_next(of, ref), true); ref = next)

#define h_list_foreach_bw_safe(list, ref, of, once, next) h_list_capture(list, of, once) \
	for(h_list_ref_type(list) ref = poor_list_last(of), next; ref && (next = poor_list_prev(of, ref), true); ref = next)

/* Declaring the meta tag first binds h_meta to this scope's meta, not to a shadowed outer list's one */
#define h_list_declare(name, tag, meta) \
	struct meta; typedef union tag { struct poor_list_node head; struct meta *h_meta; } name

/* The item typedef forces a compile error if a list is redefined with a conflicting type (which Clang's duplicate struct check misses) */
#define h_list_define(item, meta, type, member)								\
	typedef type *item;										\
	struct meta {											\
		item h_type;										\
		unsigned char h_offset[offsetof(type, member) + 1];					\
		static_assert(_Generic(&((type *)0)->member, struct poor_list_node *: 1, default: 0),	\
			      "list member (" #member ") must be an unqualified struct poor_list_node");\
	}

#define h_list_type(list) typeof_unqual(*(list)->h_meta->h_type)
#define h_list_offset(list) (sizeof((list)->h_meta->h_offset) - 1)
#define h_list_ref_type(list) typeof(_Generic((typeof(&(list)->head))nullptr,			\
	const struct poor_list_node *: (const h_list_type(list) *)nullptr, default: (h_list_type(list) *)nullptr))
#define h_list_ref(list, entry) ((h_list_ref_type(list))(entry))
#define h_list_mut_list(list) \
	static_assert_expr(!is_pointer_to_const((typeof(&(list)->head))nullptr), "list (" #list ") is const")
#define h_list_mut_head(list) (h_list_mut_list(list), &(list)->head)
#define h_list_mut_node(list, ref) \
	(static_assert_expr(!is_pointer_to_const((typeof(ref))nullptr), "entry (" #ref ") is const"), h_list_node(list, ref))
#define h_list_node(list, ref) _Generic((typeof(ref))nullptr,						\
	h_list_type(list) *: h_list_node_mut, const h_list_type(list) *: h_list_node_const)(ref, h_list_offset(list))

static inline struct poor_list_node *h_list_node_mut(void *ref, size_t offset) {
	return (void *)((unsigned char *)ref + offset);
}

static inline const struct poor_list_node *h_list_node_const(const void *ref, size_t offset) {
	return (const void *)((const unsigned char *)ref + offset);
}

static inline void h_list_init(struct poor_list_node *head) {
	head->prev = head;
	head->next = head;
}

static inline void *h_list_entry(const struct poor_list_node *head, struct poor_list_node *node, size_t offset) {
	return node == head ? nullptr : (unsigned char *)node - offset;
}

static inline void *h_list_first(const struct poor_list_node *head, size_t offset) {
	return h_list_entry(head, head->next, offset);
}

static inline void *h_list_last(const struct poor_list_node *head, size_t offset) {
	return h_list_entry(head, head->prev, offset);
}

static inline void h_list_insert(struct poor_list_node *at, struct poor_list_node *entry) {
	entry->prev = at;
	entry->next = at->next;
	at->next->prev = entry;
	at->next = entry;
}

static inline void h_list_remove(struct poor_list_node *entry) {
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
}

static inline size_t h_list_length(const struct poor_list_node *head) {
	size_t length = 0;
	for(const struct poor_list_node *node = head->next; node != head; node = node->next)
		length++;
	return length;
}

#endif // POOR_LIST_H
