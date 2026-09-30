// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_UNISTD_H
#define POOR_UNISTD_H

#include <poor_array.h>
#include <unistd.h>

#define read_array(fd, ...) read(fd, auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define write_array(fd, ...) write(fd, auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define pread_array(fd, _arrm_, offset) pread(fd, auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), offset)
#define pwrite_array(fd, _arrm_, offset) pwrite(fd, auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), offset)

#define getcwd_array(...) getcwd(auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define readlink_array(path, ...) readlink(path, auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define readlinkat_array(dirfd, path, ...) readlinkat(dirfd, path, auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define gethostname_array(...) gethostname(auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))
#define getentropy_array(...) getentropy(auto_arr(__VA_ARGS__), ARRAY_SIZE_BYTES(__VA_ARGS__))

#endif // POOR_UNISTD_H
