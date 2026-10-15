#ifndef RBTREE_SHARED_H
#define RBTREE_SHARED_H

#include <poor_rbtree.h>

struct shared_item {
	int id;
	struct poor_rbtree_node node;
};

int shared_item_cmp(const struct shared_item *a, const struct shared_item *b);

poor_rbtree_define(shared_tree, struct shared_item, node, shared_item_cmp);

bool shared_tree_insert(shared_tree *tree, struct shared_item *item);
int shared_tree_sum(const shared_tree *tree);

#endif
