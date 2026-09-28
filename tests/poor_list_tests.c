#include <poor_list.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct item {
	int id;
	struct poor_list_node link;
};

struct wide_item {
	alignas(64) int id;
	struct poor_list_node link;
	struct poor_list_node odd_link;
};

poor_list_define(item_list, struct item, link);
static_assert(sizeof(item_list) == sizeof(struct poor_list_node) && alignof(item_list) == alignof(struct poor_list_node));

struct first_item {
	struct poor_list_node link;
	int id;
};

poor_list_define(first_item_list, struct first_item, link);

struct registry {
	first_item_list firsts;
	item_list items;
};

poor_list_declare(node_list);

struct node {
	int id;
	struct poor_list_node sibling;
	node_list children;
};

poor_list_define(node_list, struct node, sibling);
static_assert(sizeof(node_list) == sizeof(struct poor_list_node) && alignof(node_list) == alignof(struct poor_list_node));

#define is_empty(list) \
	((list)->head.next == &(list)->head && (list)->head.prev == &(list)->head && poor_list_empty(list) && poor_list_length(list) == 0)

static bool has_ids(const item_list *list, size_t n, const int *ids) {
	const struct poor_list_node *head = &list->head, *node = head;
	for(size_t i = 0; i < n; i++) {
		if(node->next->prev != node)
			return false;
		node = node->next;
		if(node == head || container_of(node, struct item, link)->id != ids[i])
			return false;
	}
	return node->next == head && head->prev == node && !poor_list_empty(list) && poor_list_length(list) == n;
}

#define is_list(list, ...) has_ids(list, sizeof((int[]){__VA_ARGS__}) / sizeof(int), (int[]){__VA_ARGS__})

static void append_items(item_list *list, size_t n, struct item *items) {
	for(size_t i = 0; i < n; i++) {
		items[i].id = i + 1;
		poor_list_append(list, &items[i]);
	}
}

static int list_init_test(void) {
	static item_list static_items = POOR_LIST_INIT(static_items);
	item_list local_items = POOR_LIST_INIT(local_items);
	item_list items;
	poor_list_init(&items);

	assert(is_empty(&static_items) && is_empty(&local_items) && is_empty(&items));
	assert(!poor_list_first(&items) && !poor_list_last(&items));
	static_assert(_Generic(poor_list_empty(&items), bool: 1, default: 0));

	struct item a = {.id = 1};
	poor_list_append(&items, &a);
	poor_list_init(&items);
	assert(is_empty(&items));
	return 0;
}

static int list_insert_test(void) {
	item_list items = POOR_LIST_INIT(items);
	struct item a = {.id = 1}, b = {.id = 2}, c = {.id = 3}, d = {.id = 4}, e = {.id = 5}, f = {.id = 6}, g = {.id = 7};

	poor_list_append(&items, &a);
	assert(is_list(&items, 1));
	poor_list_prepend(&items, &b);
	assert(is_list(&items, 2, 1));
	poor_list_append(&items, &c);
	assert(is_list(&items, 2, 1, 3));
	poor_list_insert_after(&items, &b, &d);
	poor_list_insert_before(&items, &c, &e);
	assert(is_list(&items, 2, 4, 1, 5, 3));
	poor_list_insert_before(&items, &b, &f);
	poor_list_insert_after(&items, &c, &g);
	assert(is_list(&items, 6, 2, 4, 1, 5, 3, 7));
	return 0;
}

static int list_remove_test(void) {
	item_list items = POOR_LIST_INIT(items);
	struct item arr[5];
	append_items(&items, 5, arr);

	poor_list_remove(&items, &arr[2]);
	assert(is_list(&items, 1, 2, 4, 5));
	poor_list_remove(&items, &arr[0]);
	assert(is_list(&items, 2, 4, 5));
	poor_list_remove(&items, &arr[4]);
	assert(is_list(&items, 2, 4));
	poor_list_append(&items, &arr[2]);
	assert(is_list(&items, 2, 4, 3));

	poor_list_remove(&items, poor_list_first(&items));
	assert(is_list(&items, 4, 3));
	poor_list_remove(&items, poor_list_last(&items));
	assert(is_list(&items, 4));
	poor_list_remove(&items, poor_list_first(&items));
	assert(is_empty(&items));
	return 0;
}

static int list_first_last_test(void) {
	item_list items = POOR_LIST_INIT(items), empty = POOR_LIST_INIT(empty);
	struct item a = {.id = 1}, b = {.id = 2}, c = {.id = 3};

	assert(!poor_list_first(&items) && !poor_list_last(&items));
	poor_list_append(&items, &a);
	assert(poor_list_first(&items) == &a && poor_list_last(&items) == &a);
	assert(!poor_list_next(&items, &a) && !poor_list_prev(&items, &a));

	poor_list_append(&items, &b);
	poor_list_append(&items, &c);
	assert(poor_list_first(&items) == &a && poor_list_last(&items) == &c);
	assert(poor_list_next(&items, &a) == &b && poor_list_next(&items, &b) == &c && !poor_list_next(&items, &c));
	assert(poor_list_prev(&items, &c) == &b && poor_list_prev(&items, &b) == &a && !poor_list_prev(&items, &a));

	const item_list *const_items = &items;
	static_assert(_Generic(poor_list_first(&items), struct item *: 1, default: 0));
	static_assert(_Generic(poor_list_first(const_items), const struct item *: 1, default: 0));
	assert(poor_list_last(const_items) == &c && poor_list_next(const_items, poor_list_first(const_items)) == &b);

	bool use_empty = false;
	assert(poor_list_first(use_empty ? &empty : &items) == &a);
	assert(poor_list_next(use_empty ? &empty : &items, use_empty ? &a : &b) == &c);

	int popped = 0;
	struct item *first;
	while((first = poor_list_first(&items))) {
		poor_list_remove(&items, first);
		popped = popped * 10 + first->id;
	}
	assert(popped == 123 && is_empty(&items));
	return 0;
}

static int list_foreach_test(void) {
	item_list items = POOR_LIST_INIT(items), empty = POOR_LIST_INIT(empty);
	struct item arr[4];
	append_items(&items, 4, arr);

	poor_list_foreach(&empty, ref)
		assert(false);
	poor_list_foreach_bw(&empty, ref)
		assert(false);

	int visited = 0;
	poor_list_foreach(&items, ref) {
		static_assert(_Generic(ref, struct item *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	poor_list_foreach_bw(&items, ref)
		visited = visited * 10 + ref->id;
	assert(visited == 12344321);

	const item_list *const_items = &items;
	visited = 0;
	poor_list_foreach(const_items, ref) {
		static_assert(_Generic(ref, const struct item *: 1, default: 0));
		visited = visited * 10 + ref->id;
	}
	assert(visited == 1234);

	bool use_empty = false;
	visited = 0;
	poor_list_foreach(use_empty ? &empty : &items, ref)
		poor_list_foreach_bw(&items, inner)
			visited++;
	assert(visited == 16);

	visited = 0;
	poor_list_foreach(&items, ref) {
		if(ref->id % 2)
			continue;
		if(ref->id == 4)
			break;
		visited = visited * 10 + ref->id;
	}
	poor_list_foreach_bw(&items, ref)
		if((visited = visited * 10 + ref->id) > 1000)
			break;
	assert(visited == 2432);
	return 0;
}

static int list_foreach_safe_test(void) {
	item_list items = POOR_LIST_INIT(items), other = POOR_LIST_INIT(other);
	struct item arr[6];
	append_items(&items, 6, arr);

	poor_list_foreach_safe(&items, ref)
		if(ref->id % 2)
			poor_list_remove(&items, ref);
	assert(is_list(&items, 2, 4, 6));

	int visited = 0;
	poor_list_foreach_bw_safe(&items, ref) {
		visited = visited * 10 + ref->id;
		poor_list_remove(&items, ref);
		poor_list_append(&other, ref);
	}
	assert(visited == 642 && is_empty(&items) && is_list(&other, 6, 4, 2));

	poor_list_foreach_safe(&items, ref)
		assert(false);
	poor_list_foreach_bw_safe(&items, ref)
		assert(false);

	visited = 0;
	poor_list_foreach_safe(&other, x)
		poor_list_foreach_safe(&other, y)
			visited++;
	assert(visited == 9);

	visited = 0;
	poor_list_foreach_safe(&other, ref)
		if(++visited == 2)
			break;
	poor_list_foreach_bw_safe(&other, ref)
		if(++visited == 3)
			break;
	assert(visited == 3 && is_list(&other, 6, 4, 2));
	return 0;
}

#define ITER entry
#define test_cat(a, b) a##b

static int list_foreach_macro_name_test(void) {
	item_list items = POOR_LIST_INIT(items);
	struct item arr[3];
	append_items(&items, 3, arr);

	int visited = 0;
	poor_list_foreach(&items, ITER)
		visited = visited * 10 + entry->id;
	poor_list_foreach_bw(&items, test_cat(it, 1))
		visited = visited * 10 + it1->id;
	poor_list_foreach_safe(&items, ITER)
		visited = visited * 10 + ITER->id;
	poor_list_foreach_bw_safe(&items, test_cat(it, 2))
		poor_list_remove(&items, it2);
	assert(visited == 123321123 && is_empty(&items));
	return 0;
}

static int evals[3];
#define arg(i, x) (evals[i]++, (x))
#define assert_once(...) \
	(memset(evals, 0, sizeof(evals)), (void)(__VA_ARGS__), assert(evals[0] <= 1 && evals[1] <= 1 && evals[2] <= 1))

static int list_single_eval_test(void) {
	item_list lists[2];
	struct item a = {.id = 1}, b = {.id = 2}, c = {.id = 3}, *refs[] = {&a, &b, &c};

	int i = 0;
	poor_list_init(&lists[i++]);
	poor_list_init(&lists[i++]);
	assert(i == 2 && is_empty(&lists[0]) && is_empty(&lists[1]));

	assert_once(poor_list_append(arg(0, &lists[0]), arg(1, &a)));
	assert_once(poor_list_prepend(arg(0, &lists[0]), arg(1, &c)));
	assert_once(poor_list_insert_before(arg(0, &lists[0]), arg(1, &a), arg(2, &b)));
	assert_once(poor_list_remove(arg(0, &lists[0]), arg(1, &b)));
	assert_once(poor_list_insert_after(arg(0, &lists[0]), arg(1, &a), arg(2, &b)));
	assert(is_list(&lists[0], 3, 1, 2));

	assert_once(assert(poor_list_first(arg(0, &lists[0])) == &c && poor_list_last(arg(1, &lists[0])) == &b));
	assert_once(assert(poor_list_next(arg(0, &lists[0]), arg(1, &c)) == &a));
	assert_once(assert(poor_list_prev(arg(0, &lists[0]), arg(1, &a)) == &c));
	assert_once(assert(!poor_list_empty(arg(0, &lists[0])) && poor_list_length(arg(1, &lists[0])) == 3));
	assert_once(poor_list_init(arg(0, &lists[1])));

	i = 0;
	assert(poor_list_next(&lists[0], refs[i++]) == &b && i == 1);
	assert(poor_list_prev(&lists[0], refs[i++]) == &a && i == 2);

	int visited = 0;
	memset(evals, 0, sizeof(evals));
	poor_list_foreach(arg(0, &lists[0]), ref)
		visited = visited * 10 + ref->id;
	poor_list_foreach_bw(arg(1, &lists[0]), ref)
		visited = visited * 10 + ref->id;
	poor_list_foreach_safe(arg(2, &lists[0]), ref)
		visited = visited * 10 + ref->id;
	assert(evals[0] == 1 && evals[1] == 1 && evals[2] == 1 && visited == 312213312);

	memset(evals, 0, sizeof(evals));
	poor_list_foreach_bw_safe(arg(0, &lists[0]), ref)
		poor_list_remove(&lists[0], ref);
	assert(evals[0] == 1 && is_empty(&lists[0]));
	return 0;
}

static int sum_items(const item_list *list) {
	int sum = 0;
	poor_list_foreach(list, ref)
		sum += ref->id;
	return sum;
}

static int list_member_test(void) {
	struct registry r;
	poor_list_init(&r.firsts);
	poor_list_init(&r.items);
	struct first_item firsts[3] = {{.id = 1}, {.id = 2}, {.id = 3}};
	struct item items[2] = {{.id = 10}, {.id = 20}};

	for(size_t i = 0; i < 3; i++)
		poor_list_append(&r.firsts, &firsts[i]);
	poor_list_append(&r.items, &items[0]);
	poor_list_append(&r.items, &items[1]);

	int visited = 0;
	poor_list_foreach_bw(&r.firsts, ref)
		visited = visited * 10 + ref->id;
	assert(visited == 321 && poor_list_first(&r.firsts) == &firsts[0]);
	assert(sum_items(&r.items) == 30);
	return 0;
}

static int list_overaligned_test(void) {
	poor_list_define(wide_item_list, struct wide_item, link);
	poor_list_define(odd_item_list, struct wide_item, odd_link);
	alignas(64) wide_item_list all = POOR_LIST_INIT(all);
	alignas(64) odd_item_list odd = POOR_LIST_INIT(odd);
	struct wide_item items[4];

	poor_list_foreach(&all, ref)
		assert(false);
	poor_list_foreach_bw(&all, ref)
		assert(false);
	poor_list_foreach_safe(&odd, ref)
		assert(false);
	poor_list_foreach_bw_safe(&odd, ref)
		assert(false);
	assert(!poor_list_first(&all) && !poor_list_last(&odd));

	for(int i = 0; i < 4; i++) {
		items[i].id = i + 1;
		poor_list_append(&all, &items[i]);
		if(items[i].id % 2)
			poor_list_prepend(&odd, &items[i]);
	}

	int visited = 0;
	poor_list_foreach(&all, ref)
		visited = visited * 10 + ref->id;
	poor_list_foreach_bw(&odd, ref)
		visited = visited * 10 + ref->id;
	assert(visited == 123413 && poor_list_first(&odd) == &items[2]);

	poor_list_foreach_safe(&odd, ref)
		poor_list_remove(&odd, ref);
	assert(is_empty(&odd) && poor_list_length(&all) == 4);
	return 0;
}

static int walk_tree(const struct node *node, int visited) {
	visited = visited * 10 + node->id;
	poor_list_foreach(&node->children, child) {
		static_assert(_Generic(child, const struct node *: 1, default: 0));
		visited = walk_tree(child, visited);
	}
	return visited;
}

static int list_declare_test(void) {
	struct node root = {.id = 1, .children = POOR_LIST_INIT(root.children)}, nodes[4];
	for(int i = 0; i < 4; i++) {
		nodes[i].id = i + 2;
		poor_list_init(&nodes[i].children);
	}

	poor_list_append(&root.children, &nodes[0]);
	poor_list_append(&root.children, &nodes[1]);
	poor_list_append(&nodes[0].children, &nodes[2]);
	poor_list_prepend(&nodes[0].children, &nodes[3]);
	static_assert(_Generic(poor_list_first(&root.children), struct node *: 1, default: 0));
	assert(walk_tree(&root, 0) == 12543);

	poor_list_foreach_safe(&nodes[0].children, child) {
		poor_list_remove(&nodes[0].children, child);
		poor_list_append(&nodes[1].children, child);
	}
	assert(walk_tree(&root, 0) == 12354 && poor_list_empty(&nodes[0].children));
	return 0;
}

static int list_shadow_test(void) {
	poor_list_define(shadow_list, struct wide_item, link);
	shadow_list all = POOR_LIST_INIT(all);
	struct wide_item item = {.id = 1};
	poor_list_append(&all, &item);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
	{
		poor_list_define(shadow_list, struct wide_item, odd_link);
		shadow_list odd = POOR_LIST_INIT(odd);
		poor_list_append(&odd, &item);
		assert(poor_list_first(&odd) == &item && item.odd_link.prev == &odd.head);
		poor_list_remove(&odd, &item);
	}
	{
		poor_list_declare(shadow_list);
		poor_list_define(shadow_list, struct wide_item, odd_link);
		shadow_list odd = POOR_LIST_INIT(odd);
		poor_list_append(&odd, &item);
		assert(poor_list_first(&odd) == &item && item.odd_link.prev == &odd.head);
	}
#pragma GCC diagnostic pop

	assert(poor_list_first(&all) == &item && item.link.prev == &all.head && item.link.next == &all.head);
	return 0;
}

typedef int test_fn (void);

#define TEST_FN(fn) {#fn, fn}
static struct tests_struct {
	const char *test_name;
	test_fn *fn;
} tests[] = {
	TEST_FN(list_init_test),
	TEST_FN(list_insert_test),
	TEST_FN(list_remove_test),
	TEST_FN(list_first_last_test),
	TEST_FN(list_foreach_test),
	TEST_FN(list_foreach_safe_test),
	TEST_FN(list_foreach_macro_name_test),
	TEST_FN(list_single_eval_test),
	TEST_FN(list_member_test),
	TEST_FN(list_overaligned_test),
	TEST_FN(list_declare_test),
	TEST_FN(list_shadow_test),
};

int main(int argc, char **argv) {
	if(argc != 2)
		return fprintf(stderr, "usage: %s test_name\n", argv[0]), 1;

	for(size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
		if(!strcmp(argv[1], tests[i].test_name))
			return tests[i].fn();

	return fprintf(stderr, "No test found with name: \"%s\"\n", argv[1]), 1;
}
