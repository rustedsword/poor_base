#include <poor_list.h>

struct item {
	int id;
	struct poor_list_node link;
};

int main(void) {
	struct item entry = {0};
	return poor_list_node_is_linked(&entry);
}
