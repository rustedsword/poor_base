enum { rbtree_shared_burned = __COUNTER__ };

#include "rbtree_shared.h"

int shared_item_cmp(const struct shared_item *a, const struct shared_item *b) {
	return (a->id > b->id) - (a->id < b->id);
}

bool shared_tree_insert(shared_tree *tree, struct shared_item *item) {
	return !poor_rbtree_insert(tree, item);
}

int shared_tree_sum(const shared_tree *tree) {
	int sum = 0;
	poor_rbtree_foreach(tree, ref)
		sum += ref->id;
	return sum;
}
