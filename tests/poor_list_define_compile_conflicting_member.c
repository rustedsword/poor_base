#include <poor_list.h>

struct item {
	int id;
	struct poor_list link;
	struct poor_list other_link;
};

poor_list_define(item_list, struct item, link);
poor_list_define(item_list, struct item, other_link);
