#include <poor_container_of.h>

struct item {
	char c;
	int a;
};

int main(void) {
	struct item entry = {0};
	char *c = &entry.c;
	return container_of(c, struct item, a) == &entry;
}
