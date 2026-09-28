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
poor_list_define(item_list, struct other, link);
