#include <poor_meta_link.h>

static int twice(int x) { return 2 * x; }

poor_meta_link_define(key, twice);

int (*const *address)(int) = &poor_meta_link_get(key);
