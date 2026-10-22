// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2026 Aleksandrov Stanislav <lightofmysoul@gmail.com>
 */
#ifndef POOR_CONTAINER_OF_H
#define POOR_CONTAINER_OF_H
#include <stddef.h>

/*
 * container_of(ptr, type, member): get pointer to a struct by using pointer to some member of that struct
 *
 * @ptr: pointer to some member of struct
 * @type: type of struct where ptr points to
 * @member: name of struct's member
 *
 * usage:
  struct s {
	char c;
	int a;
  };

  struct s s1 = {0};
  int *a_ptr = &s1.a; // a_ptr points to member 'a' of the struct s1

  struct s *s1_ptr = container_of(a_ptr, struct s, a);

  //now s1_ptr will point to s1, so (s1_ptr == &s1) will be true

 */
#define container_of(ptr, type, member) _Generic(1, int*: (void)((ptr) - &((type *)0)->member), \
                                         default: h_container_of(ptr, type, member) )

/* Silence -Wcast-qual for the cast below: GCC doesn't take _Pragma inside expressions, so for it the rest of this file is a system header */
#ifdef __clang__
#define h_container_of_pragma(x) _Pragma(#x)
#else
#define h_container_of_pragma(x)
#pragma GCC system_header
#endif
#define h_container_of(ptr, type, member) h_container_of_pragma(clang diagnostic push) h_container_of_pragma(clang diagnostic ignored "-Wcast-qual") \
        (type *)(void *)((char *)(ptr) - offsetof(type, member)) h_container_of_pragma(clang diagnostic pop)

#endif // POOR_CONTAINER_OF_H
