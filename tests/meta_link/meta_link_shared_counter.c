enum { meta_link_shared_burned = __COUNTER__ };

#include "meta_link_shared.h"

int meta_link_shared_call(const struct shared_value *v) {
	(*poor_meta_link_get(shared_calls_key))++;
	return poor_meta_link_get(v->twice)(v->value);
}
