// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_RBTREE_H
#define POOR_RBTREE_H
#include <poor_meta_link.h>
#include <poor_traits.h>

/* A red-black tree: keeps your structs sorted and finds, inserts or removes them in O(log N).
 *
 * The tree is intrusive: you put a `struct poor_rbtree_node node;` right inside your own struct
 * and the tree links these nodes together, it never allocates anything.
 * It is type-safe: the tree type remembers what struct it holds and the function that compares them,
 * so you pass the comparator only once, when defining the tree.
 * A wrong struct doesn't compile, and a const tree gives you back const pointers.
 *
 * IMPORTANT RULES:
 *
 * 1. Define trees at file scope.
 *    poor_rbtree_define() creates a small function that calls your comparator and links it to the tree type
 *    with poor_meta_link.h, so every tree type takes one of its slots (100 per translation unit by default).
 *    Inserting and searching use that link, so do them in static inline or ordinary functions, not plain inline ones.
 *
 * 2. Keys are unique.
 *    If the tree already has an item equal to yours, poor_rbtree_insert() doesn't insert yours
 *    and returns the one in the tree instead.
 *
 * 3. Don't change the key of an item while it is in the tree.
 *    Remove the item, change it and insert it again.
 *
 * 4. Remove an item before freeing it, and remove it only once.
 *    Removing rebalances the tree and can change its root, so it needs the tree, not just the item.
 *    Finding the next or previous item can pass through items the loop has already visited,
 *    so even foreach_safe breaks if one of them was freed while still in the tree.
 *    A removed item keeps stale links, removing it again corrupts the tree.
 *
 * 5. A zero-initialized tree is empty: `job_tree jobs = {};` is ready to use.
 *
 * 6. All macro arguments are evaluated once.
 *
 * Quick example:
 *
 * 	struct job {
 * 		int id;
 * 		struct poor_rbtree_node node; // embed the tree hook here
 * 	};
 *
 * 	static int job_cmp(const struct job *a, const struct job *b) {
 * 		return (a->id > b->id) - (a->id < b->id);
 * 	}
 *
 * 	poor_rbtree_define(job_tree, struct job, node, job_cmp);
 *
 * 	job_tree jobs = {};
 * 	struct job a = {.id = 2}, b = {.id = 3}, c = {.id = 1}, d = {.id = 3};
 *
 * 	poor_rbtree_insert(&jobs, &a); // tree is now: [2]
 * 	poor_rbtree_insert(&jobs, &b); // tree is now: [2, 3]
 * 	poor_rbtree_insert(&jobs, &c); // tree is now: [1, 2, 3]
 * 	struct job *old = poor_rbtree_insert(&jobs, &d); // d isn't inserted, old is b with the same id
 *
 * 	// Loop through everything in order:
 * 	poor_rbtree_foreach(&jobs, j)
 * 		println("job ", j->id);
 * 	// prints:
 * 	// job 1
 * 	// job 2
 * 	// job 3
 *
 * 	// Searching takes an item with the key filled in:
 * 	struct job *two = poor_rbtree_find(&jobs, &(struct job){.id = 2});
 * 	poor_rbtree_remove(&jobs, two);
 * 	println(poor_rbtree_length(&jobs)); // prints: 2
 *
 * 	// Removing items inside a loop? Use foreach_safe!
 * 	poor_rbtree_foreach_safe(&jobs, j)
 * 		poor_rbtree_remove(&jobs, j);
 *
 * 	println(poor_rbtree_empty(&jobs)); // prints: true
 */

/* The node you embed inside your struct: two children and the parent,
 * which keeps the color of the node in its lowest bit. */
struct poor_rbtree_node {
	struct poor_rbtree_node *child[2];
	uintptr_t parent_color;
};

/* Defines tree type 'name' for your struct 'type', using 'member' as its node and 'cmp' to sort items.
 *
 * Your struct must be fully defined, 'member' must be a `struct poor_rbtree_node`
 * and 'cmp' a function `int cmp(const type *a, const type *b)` which returns
 * a negative number if 'a' goes before 'b', a positive one if after, and 0 if they are equal.
 * Works whether or not you called poor_rbtree_declare(name) first. */
#define poor_rbtree_define(name, type, member, cmp)								\
	poor_rbtree_declare(name); h_rbtree_define(h_rbtree_cat(h_rbtree_meta_, name), h_rbtree_cat(h_rbtree_wrap_, name),	\
		h_rbtree_cat(h_rbtree_link_, name), type, member, cmp)

/* Forward-declares tree type 'name' before its struct is complete.
 * Use this when a struct needs to hold a tree of its own type (like folders with subfolders):
 *
 *     poor_rbtree_declare(folder_tree);
 *     struct folder {
 *         char name[64];
 *         struct poor_rbtree_node node;
 *         folder_tree subfolders;
 *     };
 *     static int folder_cmp(const struct folder *a, const struct folder *b) { return strcmp(a->name, b->name); }
 *     poor_rbtree_define(folder_tree, struct folder, node, folder_cmp);
 *
 * You can embed or pass a declared tree around immediately (e.g. in headers),
 * but you must call poor_rbtree_define() once the struct is complete to access its items. */
#define poor_rbtree_declare(name) h_rbtree_declare(name, h_rbtree_cat(h_rbtree_root_, name), h_rbtree_cat(h_rbtree_meta_, name))

/* Runtime initializer. Call this to initialize or reset a tree, the items are left untouched.
 * Usage: poor_rbtree_init(&my_tree); */
#define poor_rbtree_init(tree) ((void)(*h_rbtree_mut_root(tree) = nullptr))

/* Returns true if the tree has zero items, false otherwise. */
#define poor_rbtree_empty(tree) ((bool)!(tree)->root)

/* Returns the total number of items in the tree.
 * Note: this walks through the tree (O(N)), so avoid calling it repeatedly in hot loops. */
#define poor_rbtree_length(tree) h_rbtree_length((tree)->root)

/* Navigation in sorted order: return a typed pointer to your struct, or nullptr when empty or at the end. */

/* Get the smallest item in the tree, or nullptr if empty. */
#define poor_rbtree_first(tree) h_rbtree_ref(tree, h_rbtree_edge((tree)->root, 0))

/* Get the biggest item in the tree, or nullptr if empty. */
#define poor_rbtree_last(tree)  h_rbtree_ref(tree, h_rbtree_edge((tree)->root, 1))

/* Get the item after 'ref', or nullptr if 'ref' is the last item. */
#define poor_rbtree_next(tree, ref) ((void)(tree), h_rbtree_ref(tree, h_rbtree_step(h_rbtree_node(tree, ref), 1)))

/* Get the item before 'ref', or nullptr if 'ref' is the first item. */
#define poor_rbtree_prev(tree, ref) ((void)(tree), h_rbtree_ref(tree, h_rbtree_step(h_rbtree_node(tree, ref), 0)))

/* Searching: 'key' points to an item with the fields used by the comparator filled in,
 * e.g. &(struct job){.id = 42}. It doesn't have to be in the tree. */

/* Get the item equal to 'key', or nullptr if there is none. */
#define poor_rbtree_find(tree, key) h_rbtree_find(tree, key, -1)

/* Get the smallest item that is equal to or bigger than 'key', or nullptr if there is none. */
#define poor_rbtree_find_ge(tree, key) h_rbtree_find(tree, key, 0)

/* Get the biggest item that is equal to or smaller than 'key', or nullptr if there is none. */
#define poor_rbtree_find_le(tree, key) h_rbtree_find(tree, key, 1)

/* Adding and removing items: */

/* Insert 'ref' into the tree and return nullptr.
 * If the tree already has an item equal to 'ref', returns that item and doesn't insert 'ref'. */
#define poor_rbtree_insert(tree, ref) \
	h_rbtree_ref(tree, h_rbtree_insert(h_rbtree_mut_root(tree), h_rbtree_mut_node(tree, ref), h_rbtree_offset(tree), h_rbtree_cmp(tree)))

/* Remove 'ref', which must be in the tree. Its node remains stale. */
#define poor_rbtree_remove(tree, ref) h_rbtree_remove(h_rbtree_mut_root(tree), h_rbtree_mut_node(tree, ref))

/* Loops: */

/* Loop forward from the smallest to the biggest item.
 * 'ref' is declared automatically with your struct's pointer type.
 * Warning: do NOT remove 'ref' from the tree inside this loop! */
#define poor_rbtree_foreach(tree, ref) \
	h_rbtree_foreach(tree, ref, h_rbtree_cat(h_rbtree_of_, ref), h_rbtree_cat(h_rbtree_once_, ref))

/* Loop backward from the biggest to the smallest item. Do NOT remove 'ref' inside this loop! */
#define poor_rbtree_foreach_bw(tree, ref) \
	h_rbtree_foreach_bw(tree, ref, h_rbtree_cat(h_rbtree_of_, ref), h_rbtree_cat(h_rbtree_once_, ref))

/* Loop forward, safe against removing 'ref'.
 * Use this if you plan to call poor_rbtree_remove(tree, ref) during the loop, e.g. to free every item:
 *
 *     poor_rbtree_foreach_safe(&jobs, j) {
 *         poor_rbtree_remove(&jobs, j);
 *         free(j);
 *     }
 */
#define poor_rbtree_foreach_safe(tree, ref) h_rbtree_foreach_safe(tree, ref,		\
	h_rbtree_cat(h_rbtree_of_, ref), h_rbtree_cat(h_rbtree_once_, ref), h_rbtree_cat(h_rbtree_next_, ref))

/* Loop backward, safe against removing 'ref'. */
#define poor_rbtree_foreach_bw_safe(tree, ref) h_rbtree_foreach_bw_safe(tree, ref,	\
	h_rbtree_cat(h_rbtree_of_, ref), h_rbtree_cat(h_rbtree_once_, ref), h_rbtree_cat(h_rbtree_next_, ref))

/****** ------------------------------------------------------------------------------ *******/
/*** Implementation macros. Everything below should not be used and can be changed anytime ***/
/****** ------------------------------------------------------------------------------ *******/

#define h_rbtree_cat(a, ...) h_rbtree_primitive_cat(a, __VA_ARGS__)
#define h_rbtree_primitive_cat(a, ...) a ## __VA_ARGS__

#define h_rbtree_capture(tree, of, once) for(typeof_unqual(tree) of = (tree), *once = &of; once; once = nullptr)

#define h_rbtree_foreach(tree, ref, of, once) h_rbtree_capture(tree, of, once) \
	for(h_rbtree_ref_type(tree) ref = poor_rbtree_first(of); ref; ref = poor_rbtree_next(of, ref))

#define h_rbtree_foreach_bw(tree, ref, of, once) h_rbtree_capture(tree, of, once) \
	for(h_rbtree_ref_type(tree) ref = poor_rbtree_last(of); ref; ref = poor_rbtree_prev(of, ref))

#define h_rbtree_foreach_safe(tree, ref, of, once, next) h_rbtree_capture(tree, of, once) \
	for(h_rbtree_ref_type(tree) ref = poor_rbtree_first(of), next; ref && (next = poor_rbtree_next(of, ref), true); ref = next)

#define h_rbtree_foreach_bw_safe(tree, ref, of, once, next) h_rbtree_capture(tree, of, once) \
	for(h_rbtree_ref_type(tree) ref = poor_rbtree_last(of), next; ref && (next = poor_rbtree_prev(of, ref), true); ref = next)

#define h_rbtree_declare(name, tag, meta) \
	struct meta; typedef union tag { struct poor_rbtree_node *root; struct meta *h_meta; } name

/* The comparator is checked before the wrapper calls it, so a wrong one fails with the message first */
#define h_rbtree_define(meta, wrapper, link, type, member, cmp)							\
	static_assert(_Generic(&((type *)0)->member, struct poor_rbtree_node *: 1, default: 0),			\
		      "tree member (" #member ") must be an unqualified struct poor_rbtree_node");		\
	static_assert(_Generic((cmp), int (*)(const type *, const type *): 1, default: 0),			\
		      "tree comparator (" #cmp ") must be int (const " #type " *, const " #type " *)");		\
	static inline int wrapper(const void *h_a, const void *h_b) {						\
		return (cmp)(h_a, h_b);										\
	}													\
	poor_meta_link_define(link, wrapper);									\
	struct meta {												\
		type *h_type;											\
		unsigned char h_offset[offsetof(type, member) + 1];						\
		link h_cmp;											\
	}

#define h_rbtree_type(tree) typeof_unqual(*(tree)->h_meta->h_type)
#define h_rbtree_offset(tree) (sizeof((tree)->h_meta->h_offset) - 1)
#define h_rbtree_cmp(tree) poor_meta_link_get((tree)->h_meta->h_cmp)
#define h_rbtree_is_const(tree) is_pointer_to_const((typeof(&(tree)->root))nullptr)
#define h_rbtree_ref_type(tree) typeof(_Generic((char (*)[1 + h_rbtree_is_const(tree)])nullptr,			\
	char (*)[2]: (const h_rbtree_type(tree) *)nullptr, default: (h_rbtree_type(tree) *)nullptr))
#define h_rbtree_ref(tree, node) ((h_rbtree_ref_type(tree))h_rbtree_entry(node, h_rbtree_offset(tree)))
#define h_rbtree_find(tree, key, side) h_rbtree_ref(tree,							\
	h_rbtree_search((tree)->root, h_rbtree_key(tree, key), h_rbtree_offset(tree), h_rbtree_cmp(tree), side))
#define h_rbtree_key(tree, key) \
	((void)_Generic((typeof(key))nullptr, h_rbtree_type(tree) *: 0, const h_rbtree_type(tree) *: 0), (key))

/* static_assert_expr() would break `auto x = poor_rbtree_insert(...)` on GCC, the parameter name is the message */
#define h_rbtree_assert(cond, message) ((void)sizeof(void (*)(char message[(cond) ? 1 : -1])))
#define h_rbtree_mut_root(tree) (h_rbtree_assert(!h_rbtree_is_const(tree), tree_is_const), &(tree)->root)
#define h_rbtree_mut_node(tree, ref) \
	(h_rbtree_assert(!is_pointer_to_const((typeof(ref))nullptr), entry_is_const), h_rbtree_node(tree, ref))
#define h_rbtree_node(tree, ref) _Generic((typeof(ref))nullptr,						\
	h_rbtree_type(tree) *: h_rbtree_node_mut, const h_rbtree_type(tree) *: h_rbtree_node_const)(ref, h_rbtree_offset(tree))

typedef int h_rbtree_cmp_fn(const void *, const void *);

static inline struct poor_rbtree_node *h_rbtree_node_mut(void *ref, size_t offset) {
	return (void *)((unsigned char *)ref + offset);
}

static inline const struct poor_rbtree_node *h_rbtree_node_const(const void *ref, size_t offset) {
	return (const void *)((const unsigned char *)ref + offset);
}

static inline void *h_rbtree_entry(struct poor_rbtree_node *node, size_t offset) {
	return node ? (unsigned char *)node - offset : nullptr;
}

static inline const void *h_rbtree_item(const struct poor_rbtree_node *node, size_t offset) {
	return (const unsigned char *)node - offset;
}

static inline struct poor_rbtree_node *h_rbtree_parent(const struct poor_rbtree_node *node) {
	return (struct poor_rbtree_node *)(node->parent_color & ~(uintptr_t)1);
}

static inline bool h_rbtree_is_red(const struct poor_rbtree_node *node) {
	return node && !(node->parent_color & 1);
}

static inline void h_rbtree_set_parent(struct poor_rbtree_node *node, struct poor_rbtree_node *parent) {
	node->parent_color = (uintptr_t)parent | (node->parent_color & 1);
}

static inline void h_rbtree_set_black(struct poor_rbtree_node *node) {
	node->parent_color |= 1;
}

static inline void h_rbtree_set_red(struct poor_rbtree_node *node) {
	node->parent_color &= ~(uintptr_t)1;
}

static inline void h_rbtree_change_child(struct poor_rbtree_node **root, struct poor_rbtree_node *parent,
					 const struct poor_rbtree_node *old, struct poor_rbtree_node *node) {
	*(parent ? &parent->child[parent->child[1] == old] : root) = node;
}

/* Moves 'node' down to its 'dir' side, its child from the other side takes its place */
static inline void h_rbtree_rotate(struct poor_rbtree_node **root, struct poor_rbtree_node *node, int dir) {
	struct poor_rbtree_node *up = node->child[!dir], *parent = h_rbtree_parent(node);
	if((node->child[!dir] = up->child[dir]))
		h_rbtree_set_parent(up->child[dir], node);
	up->child[dir] = node;
	h_rbtree_set_parent(node, up);
	h_rbtree_set_parent(up, parent);
	h_rbtree_change_child(root, parent, node, up);
}

static inline struct poor_rbtree_node *h_rbtree_edge(struct poor_rbtree_node *node, int dir) {
	while(node && node->child[dir])
		node = node->child[dir];
	return node;
}

static inline struct poor_rbtree_node *h_rbtree_step(const struct poor_rbtree_node *node, int dir) {
	if(node->child[dir])
		return h_rbtree_edge(node->child[dir], !dir);

	struct poor_rbtree_node *parent;
	while((parent = h_rbtree_parent(node)) && node == parent->child[dir])
		node = parent;
	return parent;
}

/* Returns the node equal to 'key', otherwise the closest bigger one for side 0,
 * the closest smaller one for side 1 and nullptr for side -1 */
static inline struct poor_rbtree_node *h_rbtree_search(struct poor_rbtree_node *node, const void *key, size_t offset,
						       h_rbtree_cmp_fn *cmp, int side) {
	struct poor_rbtree_node *closest = nullptr;
	while(node) {
		int order = cmp(key, h_rbtree_item(node, offset));
		if(!order)
			return node;
		if((order > 0) == side)
			closest = node;
		node = node->child[order > 0];
	}
	return closest;
}

static inline void h_rbtree_insert_fixup(struct poor_rbtree_node **root, struct poor_rbtree_node *node) {
	struct poor_rbtree_node *parent;
	while(h_rbtree_is_red(parent = h_rbtree_parent(node))) {
		struct poor_rbtree_node *grandparent = h_rbtree_parent(parent);
		int dir = parent == grandparent->child[1];
		struct poor_rbtree_node *uncle = grandparent->child[!dir];

		if(h_rbtree_is_red(uncle)) {
			h_rbtree_set_black(parent);
			h_rbtree_set_black(uncle);
			h_rbtree_set_red(grandparent);
			node = grandparent;
			continue;
		}

		if(node == parent->child[!dir]) {
			h_rbtree_rotate(root, parent, dir);
			parent = node;
		}
		h_rbtree_rotate(root, grandparent, !dir);
		h_rbtree_set_black(parent);
		h_rbtree_set_red(grandparent);
		break;
	}
	h_rbtree_set_black(*root);
}

static inline struct poor_rbtree_node *h_rbtree_insert(struct poor_rbtree_node **root, struct poor_rbtree_node *node,
						       size_t offset, h_rbtree_cmp_fn *cmp) {
	struct poor_rbtree_node **link = root, *parent = nullptr;
	while(*link) {
		parent = *link;
		int order = cmp(h_rbtree_item(node, offset), h_rbtree_item(parent, offset));
		if(!order)
			return parent;
		link = &parent->child[order > 0];
	}

	*node = (struct poor_rbtree_node){.parent_color = (uintptr_t)parent};
	*link = node;
	h_rbtree_insert_fixup(root, node);
	return nullptr;
}

/* 'node' replaced a removed black node, 'parent' is its parent even when 'node' is nullptr */
static inline void h_rbtree_remove_fixup(struct poor_rbtree_node **root, struct poor_rbtree_node *node,
					 struct poor_rbtree_node *parent) {
	while(parent && !h_rbtree_is_red(node)) {
		int dir = node == parent->child[1];
		struct poor_rbtree_node *sibling = parent->child[!dir];

		if(!(sibling->parent_color & 1)) {
			h_rbtree_set_black(sibling);
			h_rbtree_set_red(parent);
			h_rbtree_rotate(root, parent, dir);
			sibling = parent->child[!dir];
		}

		if(!h_rbtree_is_red(sibling->child[0]) && !h_rbtree_is_red(sibling->child[1])) {
			h_rbtree_set_red(sibling);
			node = parent;
			parent = h_rbtree_parent(node);
			continue;
		}

		if(!h_rbtree_is_red(sibling->child[!dir])) {
			h_rbtree_set_black(sibling->child[dir]);
			h_rbtree_set_red(sibling);
			h_rbtree_rotate(root, sibling, !dir);
			sibling = parent->child[!dir];
		}
		sibling->parent_color = (sibling->parent_color & ~(uintptr_t)1) | (parent->parent_color & 1);
		h_rbtree_set_black(parent);
		h_rbtree_set_black(sibling->child[!dir]);
		h_rbtree_rotate(root, parent, dir);
		return;
	}
	if(node)
		h_rbtree_set_black(node);
}

static inline void h_rbtree_remove(struct poor_rbtree_node **root, struct poor_rbtree_node *node) {
	struct poor_rbtree_node *child, *parent;
	bool black;

	if(node->child[0] && node->child[1]) {
		struct poor_rbtree_node *next = h_rbtree_edge(node->child[1], 0);
		black = !h_rbtree_is_red(next);
		child = next->child[1];
		parent = h_rbtree_parent(next);

		if(parent == node) {
			parent = next;
		} else {
			if((parent->child[0] = child))
				h_rbtree_set_parent(child, parent);
			next->child[1] = node->child[1];
			h_rbtree_set_parent(next->child[1], next);
		}
		next->child[0] = node->child[0];
		h_rbtree_set_parent(next->child[0], next);
		h_rbtree_change_child(root, h_rbtree_parent(node), node, next);
		next->parent_color = node->parent_color;
	} else {
		child = node->child[!node->child[0]];
		parent = h_rbtree_parent(node);
		black = !h_rbtree_is_red(node);
		h_rbtree_change_child(root, parent, node, child);
		if(child)
			h_rbtree_set_parent(child, parent);
	}

	if(black)
		h_rbtree_remove_fixup(root, child, parent);
}

static inline size_t h_rbtree_length(struct poor_rbtree_node *node) {
	size_t length = 0;
	for(node = h_rbtree_edge(node, 0); node; node = h_rbtree_step(node, 1))
		length++;
	return length;
}

#endif // POOR_RBTREE_H
