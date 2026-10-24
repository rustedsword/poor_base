#include <poor_array.h>

int main(void) {
	int (*null)[0] = (int (*)[0]){};
	int zero[0];
	int *views[] = {
		*arrview(0, 0, null),
		*arrview_first(0, null),
		*arrview_last(0, null),
		*arrview_shrink(0, 0, null),
		*arrview_cfront(0, null),
		*arrview_cback(0, null),
	};

	make_arrview(v, 0, 0, null);
	assert(*v == nullptr && ARRAY_SIZE(v) == 0);
	foreach_array_ref(views, ref)
		assert(*ref == nullptr);
	assert(*arrview_first(0, zero) == zero && *arrview_cback(0, zero) == zero);
	return 0;
}
