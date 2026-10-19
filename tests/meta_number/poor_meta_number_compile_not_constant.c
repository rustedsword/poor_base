#include <poor_meta_number.h>

int number(int value) {
	typedef poor_meta_number(value) runtime;
	return poor_meta_number_get(runtime);
}
