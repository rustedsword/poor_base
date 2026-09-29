#include <poor_array.h>
#include <poor_stdio.h>

/* The other macros warn about int values even without checks */
size_t static_check(int (*a)[16], unsigned char (*b)[4], size_t u, int i) {
	(void)arrview(u, u, a), (void)arrview_first(u, a), (void)arrview_last(u, a), (void)arrview_shrink(u, u, a);
	(void)arrview_cfront(u, a), (void)arrview_cback(u, a), (void)arrview_dim(u, a);
	array_insert(a, u, 5);
	array_set_bit(b, u);
	(void)arrview(i, i, a), (void)arrview_first(i, a);
	return ARRAY_SIZE(arrview(i, i, a)) + ARRAY_SIZE(arrview_first(i, a));
}

#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK

size_t runtime_check(int (*a)[16], unsigned char (*b)[4], size_t u, int i) {
	(void)arrview(u, u, a), (void)arrview_first(u, a), (void)arrview_last(u, a), (void)arrview_shrink(u, u, a);
	(void)arrview_cfront(u, a), (void)arrview_cback(u, a), (void)arrview_dim(u, a);
	array_insert(a, u, 5);
	array_set_bit(b, u);
	(void)arrview(i, i, a), (void)arrview_first(i, a);
	return ARRAY_SIZE(arrview(i, i, a)) + ARRAY_SIZE(arrview_first(i, a));
}
