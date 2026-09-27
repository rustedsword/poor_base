#include <poor_list.h>

struct item {
	int id;
	struct poor_list link;
};

typedef poor_list_of(struct item, link) item_list;

int main(void) {
	const item_list list = POOR_LIST_INIT(list);
	poor_list_init(&list);
	return 0;
}
