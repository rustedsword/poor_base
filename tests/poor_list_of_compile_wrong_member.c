#include <poor_list.h>

struct item {
	struct poor_list link;
	int id;
};

poor_list_of(struct item, id) items;
