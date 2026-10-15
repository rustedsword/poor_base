#include <poor_rbtree.h>

struct item {
	int id;
	struct poor_rbtree_node node;
};

static bool item_less(const struct item *a, const struct item *b) {
	return a->id < b->id;
}

poor_rbtree_define(item_tree, struct item, node, item_less);
