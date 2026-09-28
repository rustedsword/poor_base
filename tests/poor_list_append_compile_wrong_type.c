#include <poor_list.h>

struct item {
	int id;
	struct poor_list_node link;
};

struct other {
	int id;
	struct poor_list_node link;
};

poor_list_define(item_list, struct item, link);

int main(void) {
	item_list items = POOR_LIST_INIT(items);
	struct other entry = {0};
	poor_list_append(&items, &entry);
	return 0;
}
