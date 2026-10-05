#define POOR_META_LINK_SLOTS 1000
#include <poor_meta_link.h>
#include <assert.h>

#define BURN_10 __COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__ + \
	__COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__ + __COUNTER__
#define BURN_100 BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10 + BURN_10

enum { burned = BURN_100 + BURN_100 + BURN_100 + BURN_100 + BURN_100 + BURN_100 + BURN_100 + BURN_100 + BURN_100 };

static int twice(int x) { return 2 * x; }

poor_meta_link_define(late_key, twice);

int main(void) {
	assert(poor_meta_link_get(late_key)(21) == 42);
	return 0;
}
