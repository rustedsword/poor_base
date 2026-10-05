add_executable(poor_meta_link_tests poor_meta_link_tests.c)
target_link_libraries(poor_meta_link_tests poor_base)
target_compile_options(poor_meta_link_tests PRIVATE -Wall -Wextra -Wshadow -Werror -pedantic-errors -UNDEBUG)

foreach(test IN ITEMS meta_link_example_test meta_link_key_test meta_link_block_scope_test meta_link_object_test
        meta_link_array_test meta_link_value_test)
    add_test(NAME ${test} COMMAND poor_meta_link_tests ${test})
endforeach()

add_executable(meta_link_shared_tests meta_link_shared_tests.c meta_link_shared_counter.c meta_link_shared.c)
target_link_libraries(meta_link_shared_tests poor_base)
target_compile_options(meta_link_shared_tests PRIVATE -Wall -Wextra -Wshadow -Werror -pedantic-errors -UNDEBUG
    $<$<C_COMPILER_ID:Clang>:-Wno-c2y-extensions>)
add_test(NAME meta_link_shared_test COMMAND meta_link_shared_tests)

add_executable(poor_meta_link_slots_tests poor_meta_link_slots_tests.c)
target_link_libraries(poor_meta_link_slots_tests poor_base)
target_compile_options(poor_meta_link_slots_tests PRIVATE -Wall -Wextra -Wshadow -Werror -pedantic-errors -UNDEBUG
    $<$<C_COMPILER_ID:Clang>:-Wno-c2y-extensions>)
add_test(NAME meta_link_slots_test COMMAND poor_meta_link_slots_tests)

add_library(poor_meta_link_define_compile_out_of_slots OBJECT EXCLUDE_FROM_ALL poor_meta_link_define_compile_out_of_slots.c)
target_link_libraries(poor_meta_link_define_compile_out_of_slots poor_base)
add_test(NAME poor_meta_link_define_compile_out_of_slots COMMAND ${CMAKE_COMMAND} --build . --target poor_meta_link_define_compile_out_of_slots WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(poor_meta_link_define_compile_out_of_slots PROPERTIES PASS_REGULAR_EXPRESSION "out of slots")

add_library(poor_meta_link_get_compile_address OBJECT EXCLUDE_FROM_ALL poor_meta_link_get_compile_address.c)
target_link_libraries(poor_meta_link_get_compile_address poor_base)
add_test(NAME poor_meta_link_get_compile_address COMMAND ${CMAKE_COMMAND} --build . --target poor_meta_link_get_compile_address WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(poor_meta_link_get_compile_address PROPERTIES PASS_REGULAR_EXPRESSION "lvalue required as unary|cannot take the address of an rvalue")

add_library(poor_meta_link_slots_compile_invalid OBJECT EXCLUDE_FROM_ALL poor_meta_link_slots_compile_invalid.c)
target_link_libraries(poor_meta_link_slots_compile_invalid poor_base)
add_test(NAME poor_meta_link_slots_compile_invalid COMMAND ${CMAKE_COMMAND} --build . --target poor_meta_link_slots_compile_invalid WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(poor_meta_link_slots_compile_invalid PROPERTIES PASS_REGULAR_EXPRESSION "POOR_META_LINK_SLOTS must be")
