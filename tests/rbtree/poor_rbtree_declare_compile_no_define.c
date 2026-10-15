#include <poor_rbtree.h>

poor_rbtree_declare(node_tree);

struct node {
	struct poor_rbtree_node sibling;
	node_tree children;
};

int main(void) {
	struct node root = {};
	return !poor_rbtree_first(&root.children);
}
