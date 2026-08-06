#include <poor_array.h>

int main(void) {
	const int dest[5] = {1,2,3,4,5};
	make_arrview(view, 1, 2, dest);
	array_remove_view(dest, view);
	return 0;
}
