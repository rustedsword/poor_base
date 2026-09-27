#include <poor_list.h>

struct item {
	int id;
	const struct poor_list link;
};

poor_list_of(struct item, link) items;
