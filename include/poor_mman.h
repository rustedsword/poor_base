// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_MMAN_H
#define POOR_MMAN_H

#include <poor_array.h>
#include <sys/mman.h>

/* mmap_array(arrp, addr, prot, flags, fd, offset):
 * Assigns and returns a mapping for a pointer to an array, like malloc_array().
 * The byte length comes from the array type, including variable length arrays.
 * Pass NULL for addr to let the system choose the address.
 * On failure, arrp is set to MAP_FAILED (not NULL).
 *
 * int (*data)[100] = mmap_array(data, NULL, PROT_READ | PROT_WRITE,
 *                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
 * if(data != MAP_FAILED) {
 *     fill_array(data, 42);
 *     munmap_array(data);
 * }
 */
#define mmap_array(_arrp_, addr, prot, flags, fd, offset) \
	((_arrp_) = mmap(addr, ARRAY_SIZE_BYTES(_arrp_), prot, flags, fd, offset))

/* Accept arrays or pointers to arrays and use their byte size.
 * The underlying functions' page alignment requirements still apply.
 * Platform extensions need the same feature test macros as <sys/mman.h>.
 */
#define munmap_array(...) munmap(auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define mprotect_array(_arrm_, prot) mprotect(auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), prot)
#define msync_array(_arrm_, flags) msync(auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), flags)
#define mlock_array(...) mlock(auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define munlock_array(...) munlock(auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define madvise_array(_arrm_, advice) madvise(auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), advice)
#define posix_madvise_array(_arrm_, advice) posix_madvise(auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), advice)

#endif // POOR_MMAN_H
