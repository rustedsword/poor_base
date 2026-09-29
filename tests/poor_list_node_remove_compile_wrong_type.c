#include <poor_list.h>

struct item {
	int id;
	struct poor_list_node link;
};

int main(void) {
	struct item entry = {0};
	poor_list_node_remove(&entry);
	return 0;
}
