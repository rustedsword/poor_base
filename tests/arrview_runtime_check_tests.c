#include <limits.h>
#include <setjmp.h>

static jmp_buf rejected;
#define arr_errmsg(...) (longjmp(rejected, 1), 0)

#include <poor_array.h>
#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK

static int values[16];
static unsigned char bits[2];

static void view(size_t idx, size_t size) {
	make_arrview(v, idx, size, values);
	(void)v;
}

static void shrink(size_t skip_start, size_t skip_end) {
	make_arrview_shrink(v, skip_start, skip_end, values);
	(void)v;
}

static void first(size_t size, size_t) { (void)arrview_first(size, values); }
static void last(size_t size, size_t) { (void)arrview_last(size, values); }
static void dim(size_t size, size_t) { (void)arrview_dim(size, values); }
static void cfront(size_t skip, size_t) { (void)arrview_cfront(skip, values); }
static void cback(size_t skip, size_t) { (void)arrview_cback(skip, values); }
static void insert(size_t idx, size_t) { array_insert(values, idx, 0); }
static void set_bit(size_t idx, size_t) { array_set_bit(bits, idx); }

static void copy(size_t dst_size, size_t) {
	int dst[dst_size];
	copy_arrays(dst, values);
}

static void view_signed(int idx, int size) { (void)arrview(idx, size, values); }
static void shrink_signed(int skip_start, int skip_end) { (void)arrview_shrink(skip_start, skip_end, values); }
static void first_signed(int size, int) { (void)arrview_first(size, values); }
static void cback_signed(int skip, int) { (void)arrview_cback(skip, values); }
static void insert_signed(int idx, int) { array_insert(values, idx, 0); }
static void set_bit_signed(int idx, int) { array_set_bit(bits, idx); }

static bool accepted(void (*make)(size_t, size_t), size_t a, size_t b) {
	if(setjmp(rejected))
		return false;
	make(a, b);
	return true;
}

static bool accepted_signed(void (*make)(int, int), int a, int b) {
	if(setjmp(rejected))
		return false;
	make(a, b);
	return true;
}

static bool size_checked(void (*make)(size_t, size_t)) {
	return accepted(make, 1, 0) && accepted(make, 16, 0) && !accepted(make, 17, 0) && !accepted(make, SIZE_MAX, 0);
}

static bool index_checked(void (*make)(size_t, size_t)) {
	return accepted(make, 0, 0) && accepted(make, 15, 0) && !accepted(make, 16, 0) && !accepted(make, SIZE_MAX, 0);
}

static bool negative_rejected(void (*make)(int, int)) {
	return accepted_signed(make, 1, 1) && !accepted_signed(make, -1, 1) && !accepted_signed(make, INT_MIN, 1);
}

int main(void) {
	assert(accepted(view, 0, 16) && accepted(view, 14, 2) && accepted(view, 15, 1));
	assert(!accepted(view, 15, 2) && !accepted(view, 0, 17) && !accepted(view, 16, 1) && !accepted(view, 0, 0));
	assert(!accepted(view, SIZE_MAX, 2) && !accepted(view, 2, SIZE_MAX) && !accepted(view, SIZE_MAX, SIZE_MAX));
	assert(accepted(shrink, 0, 15) && accepted(shrink, 15, 0) && accepted(shrink, 8, 7));
	assert(!accepted(shrink, 8, 8) && !accepted(shrink, 16, 0) && !accepted(shrink, 0, 16));
	assert(!accepted(shrink, SIZE_MAX, 2) && !accepted(shrink, 2, SIZE_MAX) && !accepted(shrink, SIZE_MAX, SIZE_MAX));
	/* arrview_dim(0, ...) divides by zero in its type before the check */
	assert(size_checked(first) && size_checked(last) && size_checked(dim) && !accepted(first, 0, 0) && !accepted(last, 0, 0));
	assert(index_checked(cfront) && index_checked(cback) && index_checked(insert) && index_checked(set_bit));
	assert(accepted(copy, 16, 0) && accepted(copy, 17, 0) && !accepted(copy, 15, 0));
	assert(!accepted_signed(view_signed, 1, -1) && !accepted_signed(shrink_signed, 1, -1));
	assert(negative_rejected(view_signed) && negative_rejected(shrink_signed) && negative_rejected(first_signed));
	assert(negative_rejected(cback_signed) && negative_rejected(insert_signed) && negative_rejected(set_bit_signed));
	return 0;
}
