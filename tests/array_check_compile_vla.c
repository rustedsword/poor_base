#include <poor_array.h>
#include <poor_stdio.h>
#ifdef RUNTIME_MODE
#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK
#endif

int main(int argc, char **) {
	int vla[argc + 16];
#ifndef VLA_CASE
	(void)arrview(1, 2, vla);
	(void)arrview_first(argc, vla);
	(void)arrview_shrink(0, argc, vla);
	array_insert(vla, argc, 5);
#elif VLA_CASE == 0
	(void)arrview_first(0, vla);
#elif VLA_CASE == 1
	(void)arrview(-1, 1, vla);
#elif VLA_CASE == 2
	array_insert(vla, -1, 5);
#elif VLA_CASE == 3
	(void)arrview_cback(-1, vla);
#elif VLA_CASE == 4
	(void)arrview_shrink(-1, 0, vla);
#elif VLA_CASE == 5
	(void)arrview(0, 0, vla);
#elif VLA_CASE == 6
	array_set_bit(vla, -1);
#endif
	return 0;
}
