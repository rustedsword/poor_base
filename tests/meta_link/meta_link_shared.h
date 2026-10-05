#ifndef META_LINK_SHARED_H
#define META_LINK_SHARED_H

#include <poor_meta_link.h>

int meta_link_shared_twice(int x);
extern int meta_link_shared_calls;

poor_meta_link_define(shared_twice_key, meta_link_shared_twice);
poor_meta_link_define(shared_calls_key, &meta_link_shared_calls);

struct shared_value {
	union {
		int value;
		shared_twice_key twice;
	};
};
static_assert(sizeof(struct shared_value) == sizeof(int));

int meta_link_shared_call(const struct shared_value *v);

#endif
