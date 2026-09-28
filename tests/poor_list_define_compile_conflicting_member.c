#include <poor_list.h>

struct item {
	int id;
	struct poor_list_node link;
	struct poor_list_node other_link;
};

poor_list_define(item_list, struct item, link);
poor_list_define(item_list, struct item, other_link);
