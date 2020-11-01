add_executable(poor_map_test poor_map_test.c )
target_link_libraries(poor_map_test poor_base)

add_test(poor_map_test poor_map_test)

add_executable(printf_format printf_format.c )
target_link_libraries(printf_format poor_base)

add_test(NAME printf_fmt_char COMMAND printf_format printf_fmt_char)
add_test(NAME printf_fmt_short COMMAND printf_format printf_fmt_short)
add_test(NAME printf_fmt_int COMMAND printf_format printf_fmt_int)
add_test(NAME printf_fmt_long_int COMMAND printf_format printf_fmt_long_int)
add_test(NAME printf_fmt_llong_int COMMAND printf_format printf_fmt_llong_int)
add_test(NAME printf_fmt_str COMMAND printf_format printf_fmt_str)
add_test(NAME printf_fmt_real COMMAND printf_format printf_fmt_real)

add_test(NAME printf_fmt_hex_char COMMAND printf_format printf_fmt_hex_char)
add_test(NAME printf_fmt_hex_short COMMAND printf_format printf_fmt_hex_short)
add_test(NAME printf_fmt_hex_int COMMAND printf_format printf_fmt_hex_int)
add_test(NAME printf_fmt_hex_long COMMAND printf_format printf_fmt_hex_long)
add_test(NAME printf_fmt_hex_llong COMMAND printf_format printf_fmt_hex_llong)

add_test(NAME concat_vla_test COMMAND printf_format concat_vla_test)
add_test(NAME concat_test COMMAND printf_format concat_test)

add_test(NAME fmt_p_test COMMAND printf_format fmt_p_test)
add_test(NAME fmt_w_test COMMAND printf_format fmt_w_test)
add_test(NAME fmt_wp_test COMMAND printf_format fmt_wp_test)
add_test(NAME fmt_zw_test COMMAND printf_format fmt_zw_test)
add_test(NAME fmt_zwp_test COMMAND printf_format fmt_zwp_test)
add_test(NAME fmt_hex_fmt_test COMMAND printf_format fmt_hex_fmt_test)
