#include <poor_array.h>

static_assert(is_unsigned((unsigned char)0) && is_unsigned(0u) && is_unsigned((size_t)0) && is_unsigned(true));
static_assert(!is_unsigned(0) && !is_unsigned((signed char)0) && !is_unsigned(0LL) && !is_unsigned(0.0));
static_assert(is_unsigned((char)0) == ((char)-1 > 0));
static_assert(if_unsigned(0u, 1, 2) == 1 && if_unsigned(0, 1, 2) == 2 && if_unsigned(0.5, 1, 2) == 2);
static_assert(_Generic(if_unsigned(0u, (short)0, 0.0), short: 1, default: 0));
static_assert(!h_not_negative(-1) && !h_not_negative((signed char)-1) && !h_not_negative(-0.5));
static_assert(h_not_negative(0) && h_not_negative(0u) && h_not_negative((size_t)-1) && h_not_negative(true));

struct bits { unsigned u : 4; int s : 4; };
extern _Atomic unsigned atomic_u;
extern _Atomic int atomic_s;
static_assert(is_unsigned(((struct bits){0}).u) && !is_unsigned(((struct bits){0}).s));
static_assert(is_unsigned(atomic_u) && !is_unsigned(atomic_s));

int bitfield_and_atomic_index(int (*a)[16], struct bits b) {
	return (*arrview(b.u, 2, a))[0] + (*arrview(b.s, 2, a))[0] + (*arrview(atomic_u, 2, a))[0] + (*arrview(atomic_s, 2, a))[0];
}

int static_check_unsigned(int (*a)[16], unsigned char (*b)[4], unsigned u, size_t z) {
	make_arrview(v, z, 2, a);
	auto w = arrview(u, 2, a);
	array_insert(a, z, 5);
	array_set_bit(b, u);
	return (*v)[0] + (*w)[0] + (*arrview_cfront(u, a))[0] + (*arrview_cback(u, a))[0] + (*arrview_shrink(u, u, a))[0];
}

#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK

int runtime_check_unsigned(int (*a)[16], unsigned char (*b)[4], unsigned u, size_t z) {
	make_arrview(v, z, 2, a);
	array_insert(a, z, 5);
	array_set_bit(b, u);
	return (*v)[0] + (*arrview(u, 2, a))[0] + (*arrview_cfront(u, a))[0] + (*arrview_cback(u, a))[0] + (*arrview_shrink(u, u, a))[0];
}
