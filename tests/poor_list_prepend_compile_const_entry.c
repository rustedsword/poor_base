#include <poor_list.h>

struct item {
	int id;
	struct poor_list_node link;
};

poor_list_define(item_list, struct item, link);

int main(void) {
	item_list list = POOR_LIST_INIT(list);
	const struct item entry = {0};
	poor_list_prepend(&list, &entry);
	return 0;
}
