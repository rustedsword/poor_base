#include <poor_list.h>

struct item {
	struct poor_list_node link;
	int id;
};

poor_list_define(item_list, struct item, id);
