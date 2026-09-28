#include <poor_list.h>

poor_list_declare(node_list);

struct node {
	struct poor_list_node sibling;
	node_list children;
};

int main(void) {
	struct node root = {.children = POOR_LIST_INIT(root.children)};
	return poor_list_empty(&root.children);
}
