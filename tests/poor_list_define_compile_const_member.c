#include <poor_list.h>

struct item {
	int id;
	const struct poor_list_node link;
};

poor_list_define(item_list, struct item, link);
