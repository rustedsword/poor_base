// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_POLL_H
#define POOR_POLL_H

#include <poor_array.h>
#include <poll.h>

#define poll_array(_fdsm_, timeout) poll(auto_arr(_fdsm_), ARRAY_SIZE(_fdsm_), timeout)
#define ppoll_array(_fdsm_, timeout, sigmask) ppoll(auto_arr(_fdsm_), ARRAY_SIZE(_fdsm_), timeout, sigmask)

#endif // POOR_POLL_H
