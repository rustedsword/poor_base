#include <poor_array.h>
#include <poor_stdio.h>

/* Clang only warns about signed values when the view is passed to another array macro */
size_t static_check_signed(int (*a)[16], unsigned char (*b)[4], int i) {
	array_insert(a, i, 5);
	array_set_bit(b, i);
	return ARRAY_SIZE(arrview(i, i, a)) + ARRAY_SIZE(arrview_first(i, a)) + ARRAY_SIZE(arrview_last(i, a)) +
		ARRAY_SIZE(arrview_shrink(i, i, a)) + ARRAY_SIZE(arrview_cfront(i, a)) + ARRAY_SIZE(arrview_cback(i, a)) +
		ARRAY_SIZE(arrview_dim(i, a));
}

#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK

size_t runtime_check_signed(int (*a)[16], unsigned char (*b)[4], int i) {
	array_insert(a, i, 5);
	array_set_bit(b, i);
	return ARRAY_SIZE(arrview(i, i, a)) + ARRAY_SIZE(arrview_first(i, a)) + ARRAY_SIZE(arrview_last(i, a)) +
		ARRAY_SIZE(arrview_shrink(i, i, a)) + ARRAY_SIZE(arrview_cfront(i, a)) + ARRAY_SIZE(arrview_cback(i, a)) +
		ARRAY_SIZE(arrview_dim(i, a));
}
