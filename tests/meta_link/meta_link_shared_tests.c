#include "meta_link_shared.h"
#include <assert.h>

int main(void) {
	struct shared_value v = {.value = 21};

	assert(poor_meta_link_get(v.twice)(v.value) == 42);
	assert(meta_link_shared_call(&v) == 42);
	(*poor_meta_link_get(shared_calls_key))++;
	assert(meta_link_shared_calls == 2);
	return 0;
}
