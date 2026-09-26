option(POOR_BASE_TEST_SANITIZERS "Run tests with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

if(POOR_BASE_TEST_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_executable(poor_map_test poor_map_test.c )
target_link_libraries(poor_map_test poor_base)
target_compile_options(poor_map_test PRIVATE -Wall -Werror -UNDEBUG)

add_test(poor_map_test poor_map_test)

add_executable(printf_format printf_format.c )
target_link_libraries(printf_format poor_base)
target_compile_options(printf_format PRIVATE -Wall -Wextra -Werror -UNDEBUG)

add_executable(printf_format_struct printf_format.c )
target_link_libraries(printf_format_struct poor_base)
target_compile_definitions(printf_format_struct PRIVATE POOR_STDIO_CONSTEXPR_FORMAT=0)
target_compile_options(printf_format_struct PRIVATE -Wall -Werror -UNDEBUG)

foreach(test IN ITEMS
        printf_fmt_char printf_fmt_short printf_fmt_int printf_fmt_long_int printf_fmt_llong_int
        printf_fmt_str printf_fmt_nullptr printf_fmt_real printf_fmt_nested
        printf_fmt_hex_char printf_fmt_hex_short printf_fmt_hex_int printf_fmt_hex_long printf_fmt_hex_llong
        concat_single_eval_test concat_vla_test concat_vla_full_test concat_test
        fmt_p_test fmt_w_test fmt_wp_test fmt_zw_test fmt_zwp_test fmt_hex_fmt_test
        fprint_test)
    add_test(NAME ${test} COMMAND printf_format ${test})
    add_test(NAME ${test}_struct COMMAND printf_format_struct ${test})
endforeach()


add_executable(poor_array_tests poor_array_tests.c )
target_link_libraries(poor_array_tests poor_base)
target_compile_options(poor_array_tests PRIVATE -Wall -Werror -UNDEBUG)

add_executable(foreach_array_vla_tests foreach_array_vla_tests.c)
target_link_libraries(foreach_array_vla_tests poor_base)
target_compile_options(foreach_array_vla_tests PRIVATE -Wall -Werror -UNDEBUG)

add_executable(array_vla_function_tests array_vla_function_tests.c array_vla_function.c)
target_link_libraries(array_vla_function_tests poor_base)
target_compile_options(array_vla_function_tests PRIVATE -Wall -Werror -UNDEBUG)
# Keep the caller's values unavailable when compiling the function under test.
set_target_properties(array_vla_function_tests PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)

add_executable(arrview_runtime_check_tests arrview_runtime_check_tests.c)
target_link_libraries(arrview_runtime_check_tests poor_base)
target_compile_options(arrview_runtime_check_tests PRIVATE -Wall -Werror -UNDEBUG)

add_executable(array_insert_no_check_test array_insert_no_check_test.c)
target_link_libraries(array_insert_no_check_test poor_base)
target_compile_definitions(array_insert_no_check_test PRIVATE POOR_ARRAY_CHECK=NO_CHECK)
target_compile_options(array_insert_no_check_test PRIVATE -Wall -Werror -UNDEBUG)

if(POOR_BASE_TEST_SANITIZERS)
    # Accept GCC/Clang runtime-zero array bounds while checking actual iteration
    # with ASan and all the remaining UBSan checks.
    target_compile_options(foreach_array_vla_tests PRIVATE -fno-sanitize=vla-bound)
    target_compile_options(array_vla_function_tests PRIVATE -fno-sanitize=vla-bound)
    # Rejected views reach zero or negative VLA bounds before arr_errmsg()
    target_compile_options(arrview_runtime_check_tests PRIVATE -fno-sanitize=vla-bound)
endif()

add_test(NAME auto_arr_test COMMAND poor_array_tests auto_arr_test)
add_test(NAME array_size COMMAND poor_array_tests array_size)
add_test(NAME arrays_size COMMAND poor_array_tests arrays_size)
add_test(NAME fill_array_test COMMAND poor_array_tests fill_array_test)
add_test(NAME foreach_array_ref_test COMMAND poor_array_tests foreach_array_ref_test)
add_test(NAME foreach_array_index_test COMMAND poor_array_tests foreach_array_index_test)
add_test(NAME foreach_array_index_bw_test COMMAND poor_array_tests foreach_array_index_bw_test)
foreach(iterator IN ITEMS foreach_array_ref foreach_array_const_ref foreach_array_ref_bw foreach_array_const_ref_bw foreach_array_index foreach_array_index_bw)
    add_test(NAME ${iterator}_zero_length_test COMMAND poor_array_tests ${iterator}_zero_length_test)
    add_test(NAME ${iterator}_null_zero_length_test COMMAND poor_array_tests ${iterator}_null_zero_length_test)
    foreach(length IN ITEMS 0 1 3)
        add_test(NAME ${iterator}_vla_${length}_test COMMAND foreach_array_vla_tests ${iterator} ${length} 0)
    endforeach()
    add_test(NAME ${iterator}_null_zero_vla_test COMMAND foreach_array_vla_tests ${iterator} 0 1)
endforeach()
foreach(length IN ITEMS 0 1 3 17)
    add_test(NAME array_vla_function_${length}_test COMMAND array_vla_function_tests ${length} 0 11 3)
endforeach()
add_test(NAME array_vla_function_null_zero_test COMMAND array_vla_function_tests 0 1 -7 5)
add_test(NAME array_vla_function_negative_step_test COMMAND array_vla_function_tests 7 0 11 -2)
add_test(NAME array_vla_matrix_empty_test COMMAND array_vla_function_tests 0 3 0 11 3)
add_test(NAME array_vla_matrix_null_empty_test COMMAND array_vla_function_tests 0 3 1 -7 5)
add_test(NAME array_vla_matrix_single_test COMMAND array_vla_function_tests 1 1 0 11 3)
add_test(NAME array_vla_matrix_2x3_test COMMAND array_vla_function_tests 2 3 0 11 3)
add_test(NAME array_vla_matrix_3x2_test COMMAND array_vla_function_tests 3 2 0 -7 5)
add_test(NAME array_vla_matrix_negative_step_test COMMAND array_vla_function_tests 3 5 0 11 -2)
add_test(NAME array_vla_cube_empty_test COMMAND array_vla_function_tests 0 2 3 0 11 3)
add_test(NAME array_vla_cube_null_empty_test COMMAND array_vla_function_tests 0 2 3 1 -7 5)
add_test(NAME array_vla_cube_single_test COMMAND array_vla_function_tests 1 1 1 0 11 3)
add_test(NAME array_vla_cube_2x3x4_test COMMAND array_vla_function_tests 2 3 4 0 11 3)
add_test(NAME array_vla_cube_negative_step_test COMMAND array_vla_function_tests 3 2 1 0 11 -2)
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
add_test(NAME string_literal_test COMMAND poor_array_tests string_literal_test)
add_test(NAME array_ptr_test COMMAND poor_array_tests array_ptr_test)
add_test(NAME arrview_auto_test COMMAND poor_array_tests arrview_auto_test)
add_test(NAME array_insert_test COMMAND poor_array_tests array_insert_test)
add_test(NAME sprint_array_test COMMAND poor_array_tests sprint_array_test)
add_test(NAME print_array_test COMMAND poor_array_tests print_array_test)
set_tests_properties(print_array_test PROPERTIES PASS_REGULAR_EXPRESSION "^\\[\\]\n\\[\\]\n\\[\\]\n\\[7\\]\n\\[1,2,3\\]\n\\[0xab,0x01\\]\n\\[\\[1,2\\]\n,\\[3,4\\]\n\\]\n\\[\\[5,6\\]\n,\\[7,8\\]\n\\]\ndone\n$")
add_test(NAME arrview_runtime_check_test COMMAND arrview_runtime_check_tests)
add_test(NAME array_insert_no_check_test COMMAND array_insert_no_check_test)

add_library(concat_malloc_array_compile_pedantic OBJECT EXCLUDE_FROM_ALL concat_malloc_array_compile_pedantic.c)
target_link_libraries(concat_malloc_array_compile_pedantic poor_base)
target_compile_options(concat_malloc_array_compile_pedantic PRIVATE -pedantic-errors)
add_test(NAME concat_malloc_array_compile_pedantic COMMAND ${CMAKE_COMMAND} --build . --target concat_malloc_array_compile_pedantic WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(unsigned_index_compile_type_limits OBJECT EXCLUDE_FROM_ALL unsigned_index_compile_type_limits.c)
target_link_libraries(unsigned_index_compile_type_limits poor_base)
target_compile_options(unsigned_index_compile_type_limits PRIVATE -Wall -Wextra -Werror)
add_test(NAME unsigned_index_compile_type_limits COMMAND ${CMAKE_COMMAND} --build . --target unsigned_index_compile_type_limits WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(array_check_compile_define OBJECT EXCLUDE_FROM_ALL array_check_compile_define.c)
target_link_libraries(array_check_compile_define poor_base)
target_compile_definitions(array_check_compile_define PRIVATE POOR_ARRAY_CHECK=RUNTIME_CHECK)
target_compile_options(array_check_compile_define PRIVATE -Werror)
add_test(NAME array_check_compile_define COMMAND ${CMAKE_COMMAND} --build . --target array_check_compile_define WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#These tests should fail
add_library(array_ptr_compile_three_args OBJECT EXCLUDE_FROM_ALL array_ptr_compile_three_args.c)
target_link_libraries(array_ptr_compile_three_args poor_base)
add_test(NAME array_ptr_compile_three_args COMMAND ${CMAKE_COMMAND} --build . --target array_ptr_compile_three_args WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(array_ptr_compile_three_args PROPERTIES WILL_FAIL TRUE)

add_library(array_ptr_compile_four_args OBJECT EXCLUDE_FROM_ALL array_ptr_compile_four_args.c)
target_link_libraries(array_ptr_compile_four_args poor_base)
add_test(NAME array_ptr_compile_four_args COMMAND ${CMAKE_COMMAND} --build . --target array_ptr_compile_four_args WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(array_ptr_compile_four_args PROPERTIES WILL_FAIL TRUE)

foreach(name IN ITEMS arrview_compile_wrap_idx arrview_shrink_compile_wrap_start arrview_shrink_compile_wrap_end)
    foreach(target IN ITEMS ${name} ${name}_runtime)
        add_library(${target} OBJECT EXCLUDE_FROM_ALL ${name}.c)
        target_link_libraries(${target} poor_base)
        add_test(NAME ${target} COMMAND ${CMAKE_COMMAND} --build . --target ${target} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        set_tests_properties(${target} PROPERTIES WILL_FAIL TRUE)
    endforeach()
    target_compile_definitions(${name}_runtime PRIVATE RUNTIME_MODE)
endforeach()

add_library(arrview_compile_too_wide OBJECT EXCLUDE_FROM_ALL arrview_compile_too_wide.c)
target_link_libraries(arrview_compile_too_wide poor_base)
add_test(NAME arrview_compile_too_wide COMMAND ${CMAKE_COMMAND} --build . --target arrview_compile_too_wide WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(arrview_compile_too_wide PROPERTIES WILL_FAIL TRUE)

add_library(auto_arr_compile_ptr OBJECT EXCLUDE_FROM_ALL auto_arr_compile_ptr.c)
target_link_libraries(auto_arr_compile_ptr poor_base)
add_test(NAME auto_arr_compile_ptr COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_ptr WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_ptr PROPERTIES WILL_FAIL TRUE)

add_library(auto_arr_compile_ptr2 OBJECT EXCLUDE_FROM_ALL auto_arr_compile_ptr2.c)
target_link_libraries(auto_arr_compile_ptr2 poor_base)
add_test(NAME auto_arr_compile_ptr2 COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_ptr2 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_ptr2 PROPERTIES WILL_FAIL TRUE)

add_library(string_literal_compile_nonchar OBJECT EXCLUDE_FROM_ALL string_literal_compile_nonchar.c)
target_link_libraries(string_literal_compile_nonchar poor_base)
add_test(NAME string_literal_compile_nonchar COMMAND ${CMAKE_COMMAND} --build . --target string_literal_compile_nonchar WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(string_literal_compile_nonchar PROPERTIES WILL_FAIL TRUE)

add_library(string_literal_compile_assign OBJECT EXCLUDE_FROM_ALL string_literal_compile_assign.c)
target_link_libraries(string_literal_compile_assign poor_base)
add_test(NAME string_literal_compile_assign COMMAND ${CMAKE_COMMAND} --build . --target string_literal_compile_assign WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(string_literal_compile_assign PROPERTIES WILL_FAIL TRUE)

add_library(string_literal_compile_write OBJECT EXCLUDE_FROM_ALL string_literal_compile_write.c)
target_link_libraries(string_literal_compile_write poor_base)
add_test(NAME string_literal_compile_write COMMAND ${CMAKE_COMMAND} --build . --target string_literal_compile_write WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(string_literal_compile_write PROPERTIES WILL_FAIL TRUE)

add_library(copy_array_compile_const_dst OBJECT EXCLUDE_FROM_ALL copy_array_compile_const_dst.c)
target_link_libraries(copy_array_compile_const_dst poor_base)
add_test(NAME copy_array_compile_const_dst COMMAND ${CMAKE_COMMAND} --build . --target copy_array_compile_const_dst WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(copy_array_compile_const_dst PROPERTIES WILL_FAIL TRUE)

add_library(copy_arrays_compile_const_dst OBJECT EXCLUDE_FROM_ALL copy_arrays_compile_const_dst.c)
target_link_libraries(copy_arrays_compile_const_dst poor_base)
add_test(NAME copy_arrays_compile_const_dst COMMAND ${CMAKE_COMMAND} --build . --target copy_arrays_compile_const_dst WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(copy_arrays_compile_const_dst PROPERTIES WILL_FAIL TRUE)

add_library(memset_array_compile_const_dst OBJECT EXCLUDE_FROM_ALL memset_array_compile_const_dst.c)
target_link_libraries(memset_array_compile_const_dst poor_base)
add_test(NAME memset_array_compile_const_dst COMMAND ${CMAKE_COMMAND} --build . --target memset_array_compile_const_dst WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(memset_array_compile_const_dst PROPERTIES WILL_FAIL TRUE)

add_library(array_remove_view_compile_const_dst OBJECT EXCLUDE_FROM_ALL array_remove_view_compile_const_dst.c)
target_link_libraries(array_remove_view_compile_const_dst poor_base)
add_test(NAME array_remove_view_compile_const_dst COMMAND ${CMAKE_COMMAND} --build . --target array_remove_view_compile_const_dst WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(array_remove_view_compile_const_dst PROPERTIES WILL_FAIL TRUE)

add_library(auto_arr_compile_scalar OBJECT EXCLUDE_FROM_ALL auto_arr_compile_scalar.c)
target_link_libraries(auto_arr_compile_scalar poor_base)
add_test(NAME auto_arr_compile_scalar COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_scalar WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_scalar PROPERTIES WILL_FAIL TRUE)

add_library(auto_arr_compile_struct OBJECT EXCLUDE_FROM_ALL auto_arr_compile_struct.c)
target_link_libraries(auto_arr_compile_struct poor_base)
add_test(NAME auto_arr_compile_struct COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_struct WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_struct PROPERTIES WILL_FAIL TRUE)

add_library(auto_arr_compile_void_ptr OBJECT EXCLUDE_FROM_ALL auto_arr_compile_void_ptr.c)
target_link_libraries(auto_arr_compile_void_ptr poor_base)
add_test(NAME auto_arr_compile_void_ptr COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_void_ptr WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_void_ptr PROPERTIES WILL_FAIL TRUE)

add_library(auto_arr_compile_fn_ptr OBJECT EXCLUDE_FROM_ALL auto_arr_compile_fn_ptr.c)
target_link_libraries(auto_arr_compile_fn_ptr poor_base)
add_test(NAME auto_arr_compile_fn_ptr COMMAND ${CMAKE_COMMAND} --build . --target auto_arr_compile_fn_ptr WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(auto_arr_compile_fn_ptr PROPERTIES WILL_FAIL TRUE)
