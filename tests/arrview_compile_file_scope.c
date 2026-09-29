#include <poor_array.h>

static int values[8];
static int (*first)[2] = arrview_first(2, values);
static int (*view)[3] = arrview(1, 3, values);
static int (*shrunk)[5] = arrview_shrink(1, 2, values);

int file_scope_views(void) {
	return **first + **view + **shrunk;
}
