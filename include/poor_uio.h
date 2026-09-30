// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_UIO_H
#define POOR_UIO_H

#include <poor_array.h>
#include <sys/uio.h>

#define array_iovec(...) ((struct iovec){.iov_base = auto_arr(__VA_ARGS__), .iov_len = ARRAY_SIZE_BYTES(__VA_ARGS__)})
#define readv_array(fd, ...) readv(fd, auto_arr(__VA_ARGS__), ARRAY_SIZE(__VA_ARGS__))
#define writev_array(fd, ...) writev(fd, auto_arr(__VA_ARGS__), ARRAY_SIZE(__VA_ARGS__))
#define readv_arrays(fd, ...) readv(fd, (struct iovec[]){MAP_LIST(array_iovec, __VA_ARGS__)}, ARGS_COUNT(__VA_ARGS__))
#define writev_arrays(fd, ...) writev(fd, (struct iovec[]){MAP_LIST(h_const_iovec, __VA_ARGS__)}, ARGS_COUNT(__VA_ARGS__))

/* writev() only reads the buffers, so const arrays are fine */
#define h_const_iovec(_arrm_) {.iov_base = (void *)auto_arr(_arrm_), .iov_len = ARRAY_SIZE_BYTES(_arrm_)}

#endif // POOR_UIO_H
