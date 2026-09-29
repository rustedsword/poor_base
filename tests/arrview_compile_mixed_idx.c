#include <poor_array.h>
#include <poor_stdio.h>
#ifdef RUNTIME_MODE
#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK
#endif

int main(int argc, char **) {
	int values[16] = {0};
	(void)arrview(16, argc, values);
	return 0;
}
