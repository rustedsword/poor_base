#include <poor_list.h>

struct item {
	struct poor_list link;
	int id;
};

poor_list_define(item_list, struct item, id);
