#include "rbtree_shared.h"
#include <assert.h>

int main(void) {
	shared_tree tree = {};
	struct shared_item items[4] = {{.id = 3}, {.id = 1}, {.id = 4}, {.id = 2}};

	assert(shared_tree_insert(&tree, &items[0]) && shared_tree_insert(&tree, &items[1]));
	assert(!poor_rbtree_insert(&tree, &items[2]) && !poor_rbtree_insert(&tree, &items[3]));
	assert(!shared_tree_insert(&tree, &(struct shared_item){.id = 4}));
	assert(shared_tree_sum(&tree) == 10 && poor_rbtree_find(&tree, &(struct shared_item){.id = 2}) == &items[3]);
	return 0;
}
