#include <poor_rbtree.h>

struct item {
	int id;
	struct poor_rbtree_node node;
};

static int item_cmp(const struct item *a, const struct item *b) {
	return (a->id > b->id) - (a->id < b->id);
}

poor_rbtree_define(item_tree, struct item, node, item_cmp);

struct other {
	int id;
	struct poor_rbtree_node node;
};

int main(void) {
	item_tree items = {};
	return !poor_rbtree_find(&items, &(struct other){.id = 1});
}
