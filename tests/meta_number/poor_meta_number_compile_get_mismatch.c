#include <poor_meta_number.h>

typedef poor_meta_number(1000) wide;
static_assert(poor_meta_number_16_get(wide) == 1000);
