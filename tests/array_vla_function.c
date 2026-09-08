#include "array_vla_function.h"
#include <poor_array.h>
#include <assert.h>

/* Compile separately from the caller, with IPO disabled: the pointer, bound,
 * and other arguments must be unknown when these macros are compiled. */
long array_vla_function(size_t length, int (*array)[length], int initial, int step) {
	assert(ARRAY_SIZE(array) == length);
	assert(ARRAY_SIZE_BYTES(array) == length * sizeof(int));
	static_assert(ARRAY_ELEMENT_SIZE(array) == sizeof(int));
	assert((array_first_ref(array) == array_end_ref(array)) == (length == 0));

	fill_array(array, initial);
	size_t visited = 0;
	foreach_array_ref(array, ref) {
		static_assert(_Generic(ref, int *: true, default: false));
		assert(visited < length);
		assert((size_t)array_ref_index(array, ref) == visited);
		assert(*ref == initial);
		*ref += step * (int)visited;
		visited++;
	}
	assert(visited == length);

	visited = 0;
	foreach_array_const_ref(array, ref) {
		static_assert(_Generic(ref, const int *: true, default: false));
		assert(visited < length);
		assert(ref == &auto_arr(array)[visited]);
		assert(is_first_array_ref(array, ref) == (visited == 0));
		assert(is_last_array_ref(array, ref) == (visited + 1 == length));
		assert(*ref == initial + step * (int)visited);
		visited++;
	}
	assert(visited == length);

	size_t remaining = length;
	foreach_array_ref_bw(array, ref) {
		assert(remaining > 0);
		remaining--;
		assert((size_t)array_ref_index(array, ref) == remaining);
		assert(*ref == initial + step * (int)remaining);
		*ref += step;
	}
	assert(remaining == 0);

	remaining = length;
	foreach_array_const_ref_bw(array, ref) {
		static_assert(_Generic(ref, const int *: true, default: false));
		assert(remaining > 0);
		remaining--;
		assert(ref == &auto_arr(array)[remaining]);
		assert(*ref == initial + step * (int)(remaining + 1));
	}
	assert(remaining == 0);

	long sum = 0;
	visited = 0;
	foreach_array_index(array, index) {
		assert(visited < length);
		assert(index == visited);
		assert(auto_arr(array)[index] == initial + step * (int)(index + 1));
		sum += auto_arr(array)[index];
		visited++;
	}
	assert(visited == length);
	return sum;
}

long array_vla_matrix_function(size_t rows, size_t columns,
	int (*array)[rows][columns], int initial, int step) {
	assert(ARRAY_SIZE(array) == rows);
	assert(ARRAY_ELEMENT_SIZE(array) == columns * sizeof(int));
	assert(ARRAY_SIZE_BYTES(array) == rows * columns * sizeof(int));
	assert((array_first_ref(array) == array_end_ref(array)) == (rows == 0));

	size_t row_index = 0;
	foreach_array_ref(array, row) {
		static_assert(_Generic(row, int (*)[]: true, default: false));
		assert(row_index < rows);
		assert(row == &(*array)[row_index]);
		assert(ARRAY_SIZE(row) == columns);
		fill_array(row, initial);
		size_t column = 0;
		foreach_array_ref(row, ref) {
			assert(column < columns);
			assert(*ref == initial);
			*ref += step * (int)(row_index * columns + column);
			column++;
		}
		assert(column == columns);
		row_index++;
	}
	assert(row_index == rows);

	size_t visited = 0;
	row_index = 0;
	foreach_array_const_ref(array, row) {
		static_assert(_Generic(row, const int (*)[]: true, default: false));
		assert(row_index < rows);
		assert(is_first_array_ref(array, row) == (row_index == 0));
		assert(is_last_array_ref(array, row) == (row_index + 1 == rows));
		foreach_array_ref(row, ref) {
			static_assert(_Generic(ref, const int *: true, default: false));
			assert(visited < rows * columns);
			assert(*ref == initial + step * (int)visited);
			visited++;
		}
		row_index++;
	}
	assert(row_index == rows);
	assert(visited == rows * columns);

	size_t remaining = rows * columns;
	foreach_array_ref_bw(array, row)
		foreach_array_ref_bw(row, ref) {
			assert(remaining > 0);
			remaining--;
			assert(*ref == initial + step * (int)remaining);
			*ref += step;
		}
	assert(remaining == 0);

	remaining = rows * columns;
	foreach_array_const_ref_bw(array, row)
		foreach_array_ref_bw(row, ref) {
			static_assert(_Generic(ref, const int *: true, default: false));
			assert(remaining > 0);
			remaining--;
			assert(*ref == initial + step * (int)(remaining + 1));
		}
	assert(remaining == 0);

	long sum = 0;
	visited = 0;
	foreach_array_index(array, row)
		foreach_array_index(auto_arr(array)[row], column) {
			assert(visited < rows * columns);
			assert(row * columns + column == visited);
			sum += auto_arr(array)[row][column];
			visited++;
		}
	assert(visited == rows * columns);
	return sum;
}

long array_vla_cube_function(size_t layers, size_t rows, size_t columns,
	int (*array)[layers][rows][columns], int initial, int step) {
	assert(ARRAY_SIZE(array) == layers);
	assert(ARRAY_ELEMENT_SIZE(array) == rows * columns * sizeof(int));
	assert(ARRAY_SIZE_BYTES(array) == layers * rows * columns * sizeof(int));
	assert((array_first_ref(array) == array_end_ref(array)) == (layers == 0));

	long sum = 0;
	size_t layer = 0;
	foreach_array_ref(array, plane) {
		assert(layer < layers);
		assert(plane == &(*array)[layer]);
		assert(ARRAY_SIZE(plane) == rows);
		sum += array_vla_matrix_function(rows, columns, plane,
			initial + step * (int)(layer * rows * columns), step);
		layer++;
	}
	assert(layer == layers);

	size_t visited = 0;
	foreach_array_const_ref(array, plane)
		foreach_array_ref(plane, row)
			foreach_array_ref(row, ref) {
				static_assert(_Generic(ref, const int *: true, default: false));
				assert(visited < layers * rows * columns);
				assert(*ref == initial + step * (int)(visited + 1));
				visited++;
			}
	assert(visited == layers * rows * columns);

	size_t remaining = layers * rows * columns;
	foreach_array_ref_bw(array, plane)
		foreach_array_ref_bw(plane, row)
			foreach_array_ref_bw(row, ref) {
				static_assert(_Generic(ref, int *: true, default: false));
				assert(remaining > 0);
				remaining--;
				assert(*ref == initial + step * (int)(remaining + 1));
			}
	assert(remaining == 0);

	remaining = layers * rows * columns;
	foreach_array_const_ref_bw(array, plane)
		foreach_array_ref_bw(plane, row)
			foreach_array_ref_bw(row, ref) {
				static_assert(_Generic(ref, const int *: true, default: false));
				assert(remaining > 0);
				remaining--;
				assert(*ref == initial + step * (int)(remaining + 1));
			}
	assert(remaining == 0);

	visited = 0;
	foreach_array_index(array, index)
		assert(index == visited++);
	assert(visited == layers);
	return sum;
}
