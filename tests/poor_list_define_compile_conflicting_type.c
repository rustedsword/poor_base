#include <poor_list.h>

struct item {
	int id;
	struct poor_list link;
};

struct other {
	int id;
	struct poor_list link;
};

poor_list_define(item_list, struct item, link);
poor_list_define(item_list, struct other, link);
