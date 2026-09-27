#include <poor_list.h>

struct item {
	int id;
	struct poor_list link;
};

typedef poor_list_of(struct item, link) item_list;

int main(void) {
	item_list list = POOR_LIST_INIT(list);
	const struct item entry = {0};
	poor_list_prepend(&list, &entry);
	return 0;
}
