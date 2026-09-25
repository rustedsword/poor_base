#define arr_errmsg(...) 0
#include <poor_array.h>
#undef POOR_ARRAY_CHECK
#define POOR_ARRAY_CHECK RUNTIME_CHECK

int main(void) {
	int values[16] = {0};
	unsigned __int128 idx = 0;
	(void)arrview(idx, 2, values);
	return 0;
}
