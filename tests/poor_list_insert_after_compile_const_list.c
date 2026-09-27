#include <poor_list.h>

struct item {
	int id;
	struct poor_list link;
};

typedef poor_list_of(struct item, link) item_list;

int main(void) {
	const item_list list = POOR_LIST_INIT(list);
	struct item at = {0}, entry = {0};
	poor_list_insert_after(&list, &at, &entry);
	return 0;
}
