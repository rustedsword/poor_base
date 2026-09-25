#include <poor_array.h>
#ifdef RUNTIME_MODE
#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK
#endif

int main(void) {
	int values[16] = {0};
	(void)arrview_shrink((size_t)-1, 2, values);
	return 0;
}
