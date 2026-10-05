#include <poor_meta_link.h>

#define BURN_10 __COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__ + \
	__COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__

enum { burned = BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 };

static void fn(void) {}

poor_meta_link_define(key, fn);
