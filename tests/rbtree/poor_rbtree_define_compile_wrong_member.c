#include <poor_rbtree.h>

struct item {
	struct poor_rbtree_node node;
	int id;
};

static int item_cmp(const struct item *a, const struct item *b) {
	return (a->id > b->id) - (a->id < b->id);
}

poor_rbtree_define(item_tree, struct item, id, item_cmp);
