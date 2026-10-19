add_executable(poor_meta_number_tests poor_meta_number_tests.c)
target_link_libraries(poor_meta_number_tests poor_base)
target_compile_options(poor_meta_number_tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Werror -pedantic-errors -UNDEBUG)

foreach(test IN ITEMS meta_number_example_test meta_number_values_test meta_number_key_test meta_number_constant_test
        meta_number_layout_test)
    add_test(NAME ${test} COMMAND poor_meta_number_tests ${test})
endforeach()

function(meta_number_compile_test case regex)
    add_library(poor_meta_number_compile_${case} OBJECT EXCLUDE_FROM_ALL poor_meta_number_compile_${case}.c)
    target_link_libraries(poor_meta_number_compile_${case} poor_base)
    add_test(NAME poor_meta_number_compile_${case} COMMAND ${CMAKE_COMMAND} --build . --target poor_meta_number_compile_${case} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set_tests_properties(poor_meta_number_compile_${case} PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
endfunction()

meta_number_compile_test(wide "value must be an integer not wider than unsigned long long")
meta_number_compile_test(float "value must be an integer not wider than unsigned long long")
meta_number_compile_test(range "value must be from 0 to 2\\^16 - 1")
meta_number_compile_test(negative "value must be from 0 to 2\\^32 - 1")
meta_number_compile_test(get_mismatch "no member named.*h_digit16_0")
meta_number_compile_test(not_constant "not an integer constant expression")
