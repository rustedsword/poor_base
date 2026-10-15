#include <poor_rbtree.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct item {
	int id;
	struct poor_rbtree_node node;
};

static int item_cmp(const struct item *a, const struct item *b) {
	return (a->id > b->id) - (a->id < b->id);
}

poor_rbtree_define(item_tree, struct item, node, item_cmp);
static_assert(sizeof(item_tree) == sizeof(struct poor_rbtree_node *) && alignof(item_tree) == alignof(struct poor_rbtree_node *));

static int item_cmp_desc(const struct item *a, const struct item *b) {
	return item_cmp(b, a);
}

poor_rbtree_define(desc_tree, struct item, node, &item_cmp_desc);

struct named {
	struct poor_rbtree_node node;
	const char *name;
};

static int named_cmp(const struct named *a, const struct named *b) {
	return strcmp(a->name, b->name);
}

poor_rbtree_define(named_tree, struct named, node, named_cmp);

struct registry {
	named_tree names;
	item_tree items;
};

struct wide_item {
	alignas(64) int id;
	struct poor_rbtree_node node;
	struct poor_rbtree_node rank_node;
	int rank;
};

static int wide_id_cmp(const struct wide_item *a, const struct wide_item *b) {
	return (a->id > b->id) - (a->id < b->id);
}

static int wide_rank_cmp(const struct wide_item *a, const struct wide_item *b) {
	return (a->rank > b->rank) - (a->rank < b->rank);
}

poor_rbtree_define(wide_tree, struct wide_item, node, wide_id_cmp);
poor_rbtree_define(rank_tree, struct wide_item, rank_node, wide_rank_cmp);

poor_rbtree_declare(folder_tree);

struct folder {
	char name[16];
	struct poor_rbtree_node node;
	folder_tree subfolders;
};

static int folder_cmp(const struct folder *a, const struct folder *b) {
	return strcmp(a->name, b->name);
}

poor_rbtree_define(folder_tree, struct folder, node, folder_cmp);
static_assert(sizeof(folder_tree) == sizeof(struct poor_rbtree_node *));

#define key(n) (&(struct item){.id = (n)})

#define MAX_ITEMS 512

static const struct poor_rbtree_node *parent_of(const struct poor_rbtree_node *node) {
	return (const struct poor_rbtree_node *)(node->parent_color & ~(uintptr_t)1);
}

static bool is_red(const struct poor_rbtree_node *node) {
	return node && !(node->parent_color & 1);
}

struct walk {
	int ids[MAX_ITEMS];
	size_t n;
};

/* Returns the black height, collecting ids in order */
static int check_subtree(const struct poor_rbtree_node *node, const struct poor_rbtree_node *parent, struct walk *w) {
	if(!node)
		return 0;
	assert(parent_of(node) == parent);
	assert(!is_red(node) || (!is_red(node->child[0]) && !is_red(node->child[1])));

	int black = check_subtree(node->child[0], node, w);
	assert(w->n < MAX_ITEMS);
	w->ids[w->n++] = container_of(node, struct item, node)->id;
	assert(check_subtree(node->child[1], node, w) == black);
	return black + !is_red(node);
}

/* Checks the red-black rules, the order and the navigation macros, returns the number of items */
static size_t check_tree(const item_tree *tree, struct walk *w) {
	w->n = 0;
	assert(!is_red(tree->root));
	check_subtree(tree->root, nullptr, w);
	for(size_t i = 1; i < w->n; i++)
		assert(w->ids[i - 1] < w->ids[i]);

	size_t i = 0;
	poor_rbtree_foreach(tree, ref)
		assert(i < w->n && ref->id == w->ids[i++]);
	assert(i == w->n);
	poor_rbtree_foreach_bw(tree, ref)
		assert(i && ref->id == w->ids[--i]);
	assert(!i && poor_rbtree_length(tree) == w->n && poor_rbtree_empty(tree) == !w->n);
	return w->n;
}

static bool has_ids(const item_tree *tree, size_t n, const int *ids) {
	struct walk w;
	return check_tree(tree, &w) == n && !memcmp(w.ids, ids, n * sizeof(*ids));
}

#define is_tree(tree, ...) has_ids(tree, sizeof((int[]){__VA_ARGS__}) / sizeof(int), (int[]){__VA_ARGS__})

#define is_empty(tree) \
	(!(tree)->root && poor_rbtree_empty(tree) && !poor_rbtree_length(tree) && !poor_rbtree_first(tree) && !poor_rbtree_last(tree))

static void insert_items(item_tree *tree, size_t n, struct item *items) {
	for(size_t i = 0; i < n; i++) {
		items[i].id = i + 1;
		assert(!poor_rbtree_insert(tree, &items[i]));
	}
}

static int rbtree_init_test(void) {
	static item_tree static_items;
	item_tree local_items = {};
	item_tree items;
	poor_rbtree_init(&items);

	assert(is_empty(&static_items) && is_empty(&local_items) && is_empty(&items));
	static_assert(_Generic(poor_rbtree_empty(&items), bool: 1, default: 0));
	static_assert(_Generic(poor_rbtree_length(&items), size_t: 1, default: 0));

	struct item a = {.id = 1};
	poor_rbtree_insert(&items, &a);
	assert(is_tree(&items, 1));
	poor_rbtree_init(&items);
	assert(is_empty(&items));
	return 0;
}

static int rbtree_insert_test(void) {
	item_tree items = {};
	struct item a = {.id = 1}, b = {.id = 2}, c = {.id = 3}, d = {.id = 4}, e = {.id = 5}, f = {.id = 6}, g = {.id = 7};

	assert(!poor_rbtree_insert(&items, &d));
	assert(is_tree(&items, 4));
	assert(!poor_rbtree_insert(&items, &b) && !poor_rbtree_insert(&items, &f));
	assert(is_tree(&items, 2, 4, 6));
	assert(!poor_rbtree_insert(&items, &g) && !poor_rbtree_insert(&items, &a));
	assert(is_tree(&items, 1, 2, 4, 6, 7));
	assert(!poor_rbtree_insert(&items, &e) && !poor_rbtree_insert(&items, &c));
	assert(is_tree(&items, 1, 2, 3, 4, 5, 6, 7));

	struct item dup = {.id = 3};
	auto existing = poor_rbtree_insert(&items, &dup);
	static_assert(_Generic(existing, struct item *: 1, default: 0));
	assert(existing == &c && poor_rbtree_insert(&items, &c) == &c && poor_rbtree_insert(&items, &a) == &a);
	assert(is_tree(&items, 1, 2, 3, 4, 5, 6, 7));
	return 0;
}

static int next_id(int pattern, int i, int n) {
	switch(pattern) {
	case 0: return i;
	case 1: return n - 1 - i;
	case 2: return i % 2 ? n - 1 - i / 2 : i / 2;
	case 3: return i % 2 ? n / 2 + i / 2 : n / 2 - 1 - i / 2;
	default: return i * 37 % n;
	}
}

static int rbtree_insert_order_test(void) {
	enum { n = 128 };
	struct item items[n];
	struct walk w;

	for(int pattern = 0; pattern < 5; pattern++) {
		item_tree tree = {};
		for(int i = 0; i < n; i++) {
			items[i].id = next_id(pattern, i, n);
			assert(!poor_rbtree_insert(&tree, &items[i]));
			assert(check_tree(&tree, &w) == (size_t)i + 1);
		}
		for(int i = 0; i < n; i++)
			assert(w.ids[i] == i && poor_rbtree_insert(&tree, key(i))->id == i);
	}
	return 0;
}

static int rbtree_remove_test(void) {
	item_tree items = {};
	struct item arr[7];
	insert_items(&items, 7, arr);

	poor_rbtree_remove(&items, &arr[3]);
	assert(is_tree(&items, 1, 2, 3, 5, 6, 7));
	poor_rbtree_remove(&items, &arr[0]);
	assert(is_tree(&items, 2, 3, 5, 6, 7));
	poor_rbtree_remove(&items, &arr[6]);
	assert(is_tree(&items, 2, 3, 5, 6));
	assert(!poor_rbtree_insert(&items, &arr[3]));
	assert(is_tree(&items, 2, 3, 4, 5, 6));

	poor_rbtree_remove(&items, poor_rbtree_first(&items));
	assert(is_tree(&items, 3, 4, 5, 6));
	poor_rbtree_remove(&items, poor_rbtree_last(&items));
	assert(is_tree(&items, 3, 4, 5));
	poor_rbtree_remove(&items, container_of(items.root, struct item, node));
	assert(is_tree(&items, 3, 5));
	poor_rbtree_remove(&items, &arr[2]);
	assert(is_tree(&items, 5));
	poor_rbtree_remove(&items, &arr[4]);
	assert(is_empty(&items));
	return 0;
}

static int rbtree_remove_order_test(void) {
	enum { n = 128 };
	struct item items[n];
	struct walk w;

	for(int pattern = 0; pattern < 6; pattern++) {
		item_tree tree = {};
		insert_items(&tree, n, items);
		for(int i = 0; i < n; i++) {
			if(pattern == 5)
				poor_rbtree_remove(&tree, container_of(tree.root, struct item, node));
			else
				poor_rbtree_remove(&tree, &items[next_id(pattern, i, n)]);
			assert(check_tree(&tree, &w) == (size_t)(n - i - 1));
		}
		assert(is_empty(&tree));
	}
	return 0;
}

static int rbtree_find_test(void) {
	item_tree items = {}, empty = {};
	struct item arr[10];
	for(int i = 0; i < 10; i++) {
		arr[i].id = (i + 1) * 2;
		poor_rbtree_insert(&items, &arr[i]);
	}

	assert(!poor_rbtree_find(&empty, key(2)) && !poor_rbtree_find_ge(&empty, key(2)) && !poor_rbtree_find_le(&empty, key(2)));
	for(int i = 0; i < 10; i++) {
		assert(poor_rbtree_find(&items, key(arr[i].id)) == &arr[i]);
		assert(poor_rbtree_find_ge(&items, key(arr[i].id)) == &arr[i] && poor_rbtree_find_le(&items, key(arr[i].id)) == &arr[i]);
		assert(!poor_rbtree_find(&items, key(arr[i].id - 1)));
		assert(poor_rbtree_find_ge(&items, key(arr[i].id - 1)) == &arr[i]);
		assert(poor_rbtree_find_le(&items, key(arr[i].id + 1)) == &arr[i]);
	}
	assert(!poor_rbtree_find_ge(&items, key(21)) && !poor_rbtree_find_le(&items, key(1)));
	assert(poor_rbtree_find_ge(&items, key(-5)) == &arr[0] && poor_rbtree_find_le(&items, key(100)) == &arr[9]);

	const struct item *const_key = key(8);
	assert(poor_rbtree_find(&items, const_key) == &arr[3] && poor_rbtree_find(&items, &arr[5]) == &arr[5]);
	static_assert(_Generic(poor_rbtree_find(&items, const_key), struct item *: 1, default: 0));

	const item_tree *const_items = &items;
	static_assert(_Generic(poor_rbtree_find(const_items, key(2)), const struct item *: 1, default: 0));
	static_assert(_Generic(poor_rbtree_find_ge(const_items, key(2)), const struct item *: 1, default: 0));
	static_assert(_Generic(poor_rbtree_find_le(const_items, key(2)), const struct item *: 1, default: 0));
	assert(poor_rbtree_find_le(const_items, key(9)) == &arr[3]);
	return 0;
}

static int rbtree_first_last_test(void) {
	item_tree items = {}, empty = {};
	struct item a = {.id = 1}, b = {.id = 2}, c = {.id = 3};

	assert(!poor_rbtree_first(&items) && !poor_rbtree_last(&items));
	poor_rbtree_insert(&items, &b);
	assert(poor_rbtree_first(&items) == &b && poor_rbtree_last(&items) == &b);
	assert(!poor_rbtree_next(&items, &b) && !poor_rbtree_prev(&items, &b));

	poor_rbtree_insert(&items, &c);
	poor_rbtree_insert(&items, &a);
	assert(poor_rbtree_first(&items) == &a && poor_rbtree_last(&items) == &c);
	assert(poor_rbtree_next(&items, &a) == &b && poor_rbtree_next(&items, &b) == &c && !poor_rbtree_next(&items, &c));
	assert(poor_rbtree_prev(&items, &c) == &b && poor_rbtree_prev(&items, &b) == &a && !poor_rbtree_prev(&items, &a));

	const item_tree *const_items = &items;
	static_assert(_Generic(poor_rbtree_first(&items), struct item *: 1, default: 0));
	static_assert(_Generic(poor_rbtree_first(const_items), const struct item *: 1, default: 0));
	static_assert(_Generic(poor_rbtree_next(const_items, &a), const struct item *: 1, default: 0));
	assert(poor_rbtree_last(const_items) == &c && poor_rbtree_next(const_items, poor_rbtree_first(const_items)) == &b);

	const volatile item_tree *cv_items = &items;
	static_assert(_Generic(poor_rbtree_first(cv_items), const struct item *: 1, default: 0));
	static_assert(_Generic(poor_rbtree_find(cv_items, &b), const struct item *: 1, default: 0));
	assert(poor_rbtree_first(cv_items) == &a && poor_rbtree_prev(cv_items, &c) == &b);

	bool use_empty = false;
	assert(poor_rbtree_first(use_empty ? &empty : &items) == &a);
	assert(poor_rbtree_next(use_empty ? &empty : &items, use_empty ? &a : &b) == &c);

	int popped = 0;
	struct item *first;
	while((first = poor_rbtree_first(&items))) {
		poor_rbtree_remove(&items, first);
		popped = popped * 10 + first->id;
	}
	assert(popped == 123 && is_empty(&items));
	return 0;
}

static int rbtree_foreach_test(void) {
	item_tree items = {}, empty = {};
	struct item arr[4];
	insert_items(&items, 4, arr);

	poor_rbtree_foreach(&empty, ref)
		assert(false);
	poor_rbtree_foreach_bw(&empty, ref)
		assert(false);

	int visited = 0;
	poor_rbtree_foreach(&items, ref) {
		static_assert(_Generic(ref, struct item *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	poor_rbtree_foreach_bw(&items, ref)
		visited = visited * 10 + ref->id;
	assert(visited == 12344321);

	const item_tree *const_items = &items;
	visited = 0;
	poor_rbtree_foreach(const_items, ref) {
		static_assert(_Generic(ref, const struct item *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	const volatile item_tree *cv_items = &items;
	poor_rbtree_foreach_bw_safe(cv_items, ref) {
		static_assert(_Generic(ref, const struct item *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	assert(visited == 12344321);

	bool use_empty = false;
	visited = 0;
	poor_rbtree_foreach(use_empty ? &empty : &items, ref)
		poor_rbtree_foreach_bw(&items, inner)
			visited++;
	assert(visited == 16);

	visited = 0;
	poor_rbtree_foreach(&items, ref) {
		if(ref->id % 2)
			continue;
		if(ref->id == 4)
			break;
		visited = visited * 10 + ref->id;
	}
	poor_rbtree_foreach_bw(&items, ref)
		if((visited = visited * 10 + ref->id) > 1000)
			break;
	assert(visited == 2432);
	return 0;
}

static int rbtree_foreach_safe_test(void) {
	item_tree items = {}, other = {};
	struct item arr[6];
	insert_items(&items, 6, arr);

	poor_rbtree_foreach_safe(&items, ref)
		if(ref->id % 2)
			poor_rbtree_remove(&items, ref);
	assert(is_tree(&items, 2, 4, 6));

	int visited = 0;
	poor_rbtree_foreach_bw_safe(&items, ref) {
		visited = visited * 10 + ref->id;
		poor_rbtree_remove(&items, ref);
		ref->id *= 10;
		poor_rbtree_insert(&other, ref);
	}
	assert(visited == 642 && is_empty(&items) && is_tree(&other, 20, 40, 60));

	poor_rbtree_foreach_safe(&items, ref)
		assert(false);
	poor_rbtree_foreach_bw_safe(&items, ref)
		assert(false);

	visited = 0;
	poor_rbtree_foreach_safe(&other, x)
		poor_rbtree_foreach_safe(&other, y)
			visited++;
	assert(visited == 9);

	visited = 0;
	poor_rbtree_foreach_safe(&other, ref)
		if(++visited == 2)
			break;
	poor_rbtree_foreach_bw_safe(&other, ref)
		if(++visited == 3)
			break;
	assert(visited == 3 && is_tree(&other, 20, 40, 60));
	return 0;
}

#define ITER entry
#define test_cat(a, b) a##b

static int rbtree_foreach_macro_name_test(void) {
	item_tree items = {};
	struct item arr[3];
	insert_items(&items, 3, arr);

	int visited = 0;
	poor_rbtree_foreach(&items, ITER)
		visited = visited * 10 + entry->id;
	poor_rbtree_foreach_bw(&items, test_cat(it, 1))
		visited = visited * 10 + it1->id;
	poor_rbtree_foreach_safe(&items, ITER)
		visited = visited * 10 + ITER->id;
	poor_rbtree_foreach_bw_safe(&items, test_cat(it, 2))
		poor_rbtree_remove(&items, it2);
	assert(visited == 123321123 && is_empty(&items));
	return 0;
}

static int evals[3];
#define arg(i, x) (evals[i]++, (x))
#define assert_once(...) \
	(memset(evals, 0, sizeof(evals)), (void)(__VA_ARGS__), assert(evals[0] == 1 && evals[1] <= 1 && evals[2] <= 1))

static int rbtree_single_eval_test(void) {
	item_tree trees[2];
	struct item a = {.id = 1}, b = {.id = 2}, c = {.id = 3}, *refs[] = {&a, &b, &c};

	int i = 0;
	poor_rbtree_init(&trees[i++]);
	poor_rbtree_init(&trees[i++]);
	assert(i == 2 && is_empty(&trees[0]) && is_empty(&trees[1]));

	assert_once(poor_rbtree_insert(arg(0, &trees[0]), arg(1, &a)));
	assert_once(poor_rbtree_insert(arg(0, &trees[0]), arg(1, &c)));
	assert_once(poor_rbtree_insert(arg(0, &trees[0]), arg(1, &b)));
	assert_once(poor_rbtree_remove(arg(0, &trees[0]), arg(1, &b)));
	assert_once(assert(!poor_rbtree_insert(arg(0, &trees[0]), arg(1, &b))));
	assert(is_tree(&trees[0], 1, 2, 3));

	assert_once(assert(poor_rbtree_first(arg(0, &trees[0])) == &a && poor_rbtree_last(arg(1, &trees[0])) == &c));
	assert_once(assert(poor_rbtree_next(arg(0, &trees[0]), arg(1, &a)) == &b));
	assert_once(assert(poor_rbtree_prev(arg(0, &trees[0]), arg(1, &c)) == &b));
	assert_once(assert(poor_rbtree_find(arg(0, &trees[0]), arg(1, key(2))) == &b));
	assert_once(assert(poor_rbtree_find_ge(arg(0, &trees[0]), arg(1, key(0))) == &a));
	assert_once(assert(poor_rbtree_find_le(arg(0, &trees[0]), arg(1, key(5))) == &c));
	assert_once(assert(!poor_rbtree_empty(arg(0, &trees[0])) && poor_rbtree_length(arg(1, &trees[0])) == 3));
	assert_once(poor_rbtree_init(arg(0, &trees[1])));

	i = 0;
	assert(poor_rbtree_next(&trees[0], refs[i++]) == &b && i == 1);
	assert(poor_rbtree_find(&trees[0], refs[i++]) == &b && i == 2);
	assert(poor_rbtree_insert(&trees[0], refs[i++]) == &c && i == 3);

	int visited = 0;
	memset(evals, 0, sizeof(evals));
	poor_rbtree_foreach(arg(0, &trees[0]), ref)
		visited = visited * 10 + ref->id;
	poor_rbtree_foreach_bw(arg(1, &trees[0]), ref)
		visited = visited * 10 + ref->id;
	poor_rbtree_foreach_safe(arg(2, &trees[0]), ref)
		visited = visited * 10 + ref->id;
	assert(evals[0] == 1 && evals[1] == 1 && evals[2] == 1 && visited == 123321123);

	memset(evals, 0, sizeof(evals));
	poor_rbtree_foreach_bw_safe(arg(0, &trees[0]), ref)
		poor_rbtree_remove(&trees[0], ref);
	assert(evals[0] == 1 && is_empty(&trees[0]));
	return 0;
}

static unsigned long long rng_state = 0x9e3779b97f4a7c15ULL;

static unsigned rng(unsigned n) {
	rng_state ^= rng_state << 13;
	rng_state ^= rng_state >> 7;
	rng_state ^= rng_state << 17;
	return rng_state % n;
}

static int rbtree_random_test(void) {
	enum { n = 300 };
	static struct item items[n];
	bool present[n] = {};
	size_t count = 0;
	item_tree tree = {};
	struct walk w;

	for(int i = 0; i < n; i++)
		items[i].id = i * 2;

	for(int op = 0; op < 30000; op++) {
		unsigned i = rng(n);
		if(present[i]) {
			poor_rbtree_remove(&tree, &items[i]);
			count--;
		} else {
			assert(!poor_rbtree_insert(&tree, &items[i]));
			count++;
		}
		present[i] = !present[i];
		assert(check_tree(&tree, &w) == count);

		int k = (int)rng(2 * n + 1) - 1;
		struct item *found = poor_rbtree_find(&tree, key(k)), *ge = poor_rbtree_find_ge(&tree, key(k)), *le = poor_rbtree_find_le(&tree, key(k));
		assert(found == (k >= 0 && k % 2 == 0 && present[k / 2] ? &items[k / 2] : nullptr));

		struct item *want_ge = nullptr, *want_le = nullptr;
		for(int j = 0; j < n; j++) {
			if(present[j] && items[j].id >= k && !want_ge)
				want_ge = &items[j];
			if(present[j] && items[j].id <= k)
				want_le = &items[j];
		}
		assert(ge == want_ge && le == want_le);
	}

	poor_rbtree_foreach_safe(&tree, ref) {
		poor_rbtree_remove(&tree, ref);
		assert(check_tree(&tree, &w) == --count);
	}
	assert(!count && is_empty(&tree));
	return 0;
}

static int sum_items(const item_tree *tree) {
	int sum = 0;
	poor_rbtree_foreach(tree, ref)
		sum += ref->id;
	return sum;
}

static int rbtree_member_test(void) {
	struct registry r = {};
	struct named names[3] = {{.name = "beta"}, {.name = "gamma"}, {.name = "alpha"}};
	struct item items[2] = {{.id = 10}, {.id = 20}};

	for(size_t i = 0; i < 3; i++)
		poor_rbtree_insert(&r.names, &names[i]);
	poor_rbtree_insert(&r.items, &items[0]);
	poor_rbtree_insert(&r.items, &items[1]);

	char order[32] = "";
	poor_rbtree_foreach(&r.names, ref)
		strcat(strcat(order, ref->name), " ");
	assert(!strcmp(order, "alpha beta gamma "));
	assert(poor_rbtree_find(&r.names, &(struct named){.name = "gamma"}) == &names[1]);
	assert(poor_rbtree_find_ge(&r.names, &(struct named){.name = "b"}) == &names[0]);
	assert(!poor_rbtree_find(&r.names, &(struct named){.name = "delta"}));
	assert(poor_rbtree_insert(&r.names, &(struct named){.name = "alpha"}) == &names[2]);
	assert(sum_items(&r.items) == 30);

	desc_tree desc = {};
	struct item arr[4];
	for(int i = 0; i < 4; i++) {
		arr[i].id = i + 1;
		poor_rbtree_insert(&desc, &arr[i]);
	}
	int visited = 0;
	poor_rbtree_foreach(&desc, ref)
		visited = visited * 10 + ref->id;
	assert(visited == 4321 && poor_rbtree_find_ge(&desc, key(5)) == &arr[3] && poor_rbtree_find_le(&desc, key(0)) == &arr[0]);
	return 0;
}

static int rbtree_overaligned_test(void) {
	alignas(64) wide_tree by_id = {};
	alignas(64) rank_tree by_rank = {};
	struct wide_item items[4];

	poor_rbtree_foreach(&by_id, ref)
		assert(false);
	poor_rbtree_foreach_bw(&by_id, ref)
		assert(false);
	poor_rbtree_foreach_safe(&by_rank, ref)
		assert(false);
	poor_rbtree_foreach_bw_safe(&by_rank, ref)
		assert(false);
	assert(!poor_rbtree_first(&by_id) && !poor_rbtree_last(&by_rank));

	for(int i = 0; i < 4; i++) {
		items[i].id = i + 1;
		items[i].rank = (i * 3) % 4;
		poor_rbtree_insert(&by_id, &items[i]);
		poor_rbtree_insert(&by_rank, &items[i]);
	}

	int visited = 0;
	poor_rbtree_foreach(&by_id, ref)
		visited = visited * 10 + ref->id;
	poor_rbtree_foreach(&by_rank, ref)
		visited = visited * 10 + ref->id;
	assert(visited == 12341432);
	assert(poor_rbtree_find(&by_rank, &(struct wide_item){.rank = 2}) == &items[2]);

	poor_rbtree_foreach_safe(&by_rank, ref)
		poor_rbtree_remove(&by_rank, ref);
	assert(poor_rbtree_empty(&by_rank) && poor_rbtree_length(&by_id) == 4);
	return 0;
}

static void walk_folders(const struct folder *folder, char *out) {
	strcat(strcat(out, folder->name), "(");
	poor_rbtree_foreach(&folder->subfolders, child) {
		static_assert(_Generic(child, const struct folder *: 1, default: 0));
		walk_folders(child, out);
	}
	strcat(out, ")");
}

static int rbtree_declare_test(void) {
	struct folder root = {.name = "root"}, folders[4] = {{.name = "usr"}, {.name = "etc"}, {.name = "lib"}, {.name = "bin"}};

	poor_rbtree_insert(&root.subfolders, &folders[0]);
	poor_rbtree_insert(&root.subfolders, &folders[1]);
	poor_rbtree_insert(&folders[0].subfolders, &folders[2]);
	poor_rbtree_insert(&folders[0].subfolders, &folders[3]);
	static_assert(_Generic(poor_rbtree_first(&root.subfolders), struct folder *: 1, default: 0));

	char out[64] = "";
	walk_folders(&root, out);
	assert(!strcmp(out, "root(etc()usr(bin()lib()))"));

	poor_rbtree_foreach_safe(&folders[0].subfolders, child) {
		poor_rbtree_remove(&folders[0].subfolders, child);
		poor_rbtree_insert(&folders[1].subfolders, child);
	}
	out[0] = '\0';
	walk_folders(&root, out);
	assert(!strcmp(out, "root(etc(bin()lib())usr())") && poor_rbtree_empty(&folders[0].subfolders));
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(rbtree_init_test),
	TEST_FN(rbtree_insert_test),
	TEST_FN(rbtree_insert_order_test),
	TEST_FN(rbtree_remove_test),
	TEST_FN(rbtree_remove_order_test),
	TEST_FN(rbtree_find_test),
	TEST_FN(rbtree_first_last_test),
	TEST_FN(rbtree_foreach_test),
	TEST_FN(rbtree_foreach_safe_test),
	TEST_FN(rbtree_foreach_macro_name_test),
	TEST_FN(rbtree_single_eval_test),
	TEST_FN(rbtree_random_test),
	TEST_FN(rbtree_member_test),
	TEST_FN(rbtree_overaligned_test),
	TEST_FN(rbtree_declare_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
