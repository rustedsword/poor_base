#include <poor_list.h>

struct item {
	int id;
	struct poor_list link;
};

typedef poor_list_of(struct item, link) item_list;

int main(void) {
	const item_list list = POOR_LIST_INIT(list);
	struct item entry = {0};
	poor_list_append(&list, &entry);
	return 0;
}
