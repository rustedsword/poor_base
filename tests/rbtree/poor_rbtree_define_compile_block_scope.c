#include <poor_rbtree.h>

struct item {
	int id;
	struct poor_rbtree_node node;
};

static int item_cmp(const struct item *a, const struct item *b) {
	return (a->id > b->id) - (a->id < b->id);
}

int main(void) {
	poor_rbtree_define(item_tree, struct item, node, item_cmp);
	item_tree items = {};
	return !poor_rbtree_empty(&items);
}
