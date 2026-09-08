#ifndef ARRAY_VLA_FUNCTION_H
#define ARRAY_VLA_FUNCTION_H

#include <stddef.h>

long array_vla_function(size_t length, int (*array)[length], int initial, int step);
long array_vla_matrix_function(size_t rows, size_t columns,
	int (*array)[rows][columns], int initial, int step);
long array_vla_cube_function(size_t layers, size_t rows, size_t columns,
	int (*array)[layers][rows][columns], int initial, int step);

#endif
