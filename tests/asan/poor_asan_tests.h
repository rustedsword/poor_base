#ifndef POOR_ASAN_TESTS_H
#define POOR_ASAN_TESTS_H

/* Expectations follow the compiler, including sanitizer flags supplied outside CMake. */
#if defined(__SANITIZE_ADDRESS__)
constexpr bool asan_enabled = true;
#elif defined(__has_feature)
constexpr bool asan_enabled = __has_feature(address_sanitizer);
#else
constexpr bool asan_enabled = false;
#endif

#endif // POOR_ASAN_TESTS_H
