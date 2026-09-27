#include <poor_list.h>

struct item {
	int id;
	struct poor_list link;
};

struct other {
	int id;
	struct poor_list link;
};

int main(void) {
	poor_list_of(struct item, link) items = POOR_LIST_INIT(items);
	struct other entry = {0};
	poor_list_append(&items, &entry);
	return 0;
}
