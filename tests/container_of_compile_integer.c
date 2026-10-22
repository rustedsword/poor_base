#include <poor_container_of.h>
#include <stdint.h>

struct item {
	char c;
	int a;
};

int main(void) {
	struct item entry = {0};
	intptr_t addr = (intptr_t)&entry.a;
	return container_of(addr, struct item, a) == &entry;
}
