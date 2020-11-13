add_executable(poor_map_test poor_map_test.c )
target_link_libraries(poor_map_test poor_base)

add_test(poor_map_test poor_map_test)

add_executable(printf_format printf_format.c )
target_link_libraries(printf_format poor_base)
target_compile_options(printf_format PUBLIC -Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-but-set-variable)

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

add_test(NAME fprint_test COMMAND printf_format fprint_test)


add_executable(poor_array_tests poor_array_tests.c )
target_link_libraries(poor_array_tests poor_base)
#no-ignored-qualifiers is for TYPEOF_NO_QUAL()
target_compile_options(poor_array_tests PUBLIC -Wall -Wextra -Werror -Wno-ignored-qualifiers -Wno-unused-variable -Wno-unused-but-set-variable)

add_test(NAME auto_arr_test COMMAND poor_array_tests auto_arr_test)
add_test(NAME array_size COMMAND poor_array_tests array_size)
add_test(NAME arrays_size COMMAND poor_array_tests arrays_size)
add_test(NAME fill_array_test COMMAND poor_array_tests fill_array_test)
add_test(NAME array_accessors COMMAND poor_array_tests array_accessors)
add_test(NAME copy_array_single COMMAND poor_array_tests copy_array_single)
add_test(NAME copy_array_multiple COMMAND poor_array_tests copy_array_multiple)
add_test(NAME same_type_arrays COMMAND poor_array_tests same_type_arrays)
add_test(NAME merged_array_test COMMAND poor_array_tests merged_array_test)
add_test(NAME arrview_simple COMMAND poor_array_tests arrview_simple)
add_test(NAME arrview_first_test COMMAND poor_array_tests arrview_first_test)
add_test(NAME arrview_last_test COMMAND poor_array_tests arrview_last_test)
add_test(NAME arrview_shrink_test COMMAND poor_array_tests arrview_shrink_test)
add_test(NAME array_dim_flat_test COMMAND poor_array_tests array_dim_flat_test)

#These tests should fail
add_library(auto_arr_compile_ptr OBJECT EXCLUDE_FROM_ALL auto_arr_compile_ptr.c)
target_link_libraries(auto_arr_compile_ptr poor_base)
add_test(NAME auto_arr_compile_ptr COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_ptr WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_ptr PROPERTIES WILL_FAIL TRUE)

add_library(auto_arr_compile_ptr2 OBJECT EXCLUDE_FROM_ALL auto_arr_compile_ptr2.c)
target_link_libraries(auto_arr_compile_ptr2 poor_base)
add_test(NAME auto_arr_compile_ptr2 COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_ptr2 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_ptr2 PROPERTIES WILL_FAIL TRUE)
