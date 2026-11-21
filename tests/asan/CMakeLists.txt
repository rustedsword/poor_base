add_executable(poor_asan_tests poor_asan_tests.c poor_asan_interface_tests.c)
target_link_libraries(poor_asan_tests poor_base)
target_compile_options(poor_asan_tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Wundef -Wredundant-decls
    -Werror -pedantic-errors -UNDEBUG)

foreach(test IN ITEMS asan_region_test asan_object_test asan_vla_test asan_single_eval_test asan_interface_test)
    add_test(NAME ${test} COMMAND poor_asan_tests ${test})
endforeach()

add_test(NAME asan_use_after_poison_test COMMAND poor_asan_tests asan_use_after_poison_test)
set_tests_properties(asan_use_after_poison_test PROPERTIES PASS_REGULAR_EXPRESSION "use-after-poison" SKIP_RETURN_CODE 77)

function(asan_compile_test case regex)
    add_library(poor_asan_compile_${case} OBJECT EXCLUDE_FROM_ALL poor_asan_compile_${case}.c)
    target_link_libraries(poor_asan_compile_${case} poor_base)
    add_test(NAME poor_asan_compile_${case} COMMAND ${CMAKE_COMMAND} --build . --target poor_asan_compile_${case} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set_tests_properties(poor_asan_compile_${case} PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
endfunction()

asan_compile_test(void_ptr "object \\(block\\) must not be a pointer to void")
asan_compile_test(integer_addr "makes pointer from integer|incompatible integer to pointer conversion")
