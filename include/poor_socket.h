// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_SOCKET_H
#define POOR_SOCKET_H

#include <poor_array.h>
#include <sys/socket.h>

#define recv_array(fd, _arrm_, flags) recv(fd, auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), flags)
#define send_array(fd, _arrm_, flags) send(fd, auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), flags)
#define recvfrom_array(fd, _arrm_, flags, addr, addrlen) \
	recvfrom(fd, auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), flags, addr, addrlen)
#define sendto_array(fd, _arrm_, flags, addr, addrlen) \
	sendto(fd, auto_arr(_arrm_), ARRAY_SIZE_BYTES(_arrm_), flags, addr, addrlen)

#endif // POOR_SOCKET_H
