#include <poor_list.h>

struct item {
	int id;
	struct poor_list_node link;
};

poor_list_define(item_list, struct item, link);

int main(void) {
	const item_list list = POOR_LIST_INIT(list);
	struct item entry = {0};
	poor_list_append(&list, &entry);
	return 0;
}
