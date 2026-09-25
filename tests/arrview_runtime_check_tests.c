#include <setjmp.h>

static jmp_buf rejected;
#define arr_errmsg(...) (longjmp(rejected, 1), 0)

#include <poor_array.h>
#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK

static int values[16];

static void view(size_t idx, size_t size) {
	make_arrview(v, idx, size, values);
	(void)v;
}

static void shrink(size_t skip_start, size_t skip_end) {
	make_arrview_shrink(v, skip_start, skip_end, values);
	(void)v;
}

static bool accepted(void (*make)(size_t, size_t), size_t a, size_t b) {
	if(setjmp(rejected))
		return false;
	make(a, b);
	return true;
}

int main(void) {
	assert(accepted(view, 0, 16) && accepted(view, 14, 2) && accepted(view, 15, 1));
	assert(!accepted(view, 15, 2) && !accepted(view, 0, 17) && !accepted(view, 16, 1));
	assert(!accepted(view, SIZE_MAX, 2) && !accepted(view, 2, SIZE_MAX) && !accepted(view, SIZE_MAX, SIZE_MAX));
	assert(accepted(shrink, 0, 15) && accepted(shrink, 15, 0) && accepted(shrink, 8, 7));
	assert(!accepted(shrink, 8, 8) && !accepted(shrink, 16, 0) && !accepted(shrink, 0, 16));
	assert(!accepted(shrink, SIZE_MAX, 2) && !accepted(shrink, 2, SIZE_MAX) && !accepted(shrink, SIZE_MAX, SIZE_MAX));
	return 0;
}
