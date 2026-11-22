find_package(Threads REQUIRED)

add_executable(poor_fan_tests poor_fan_tests.c)
target_link_libraries(poor_fan_tests poor_base Threads::Threads)
target_compile_options(poor_fan_tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Wundef -Werror -pedantic-errors -UNDEBUG)

foreach(test IN ITEMS fan_init_test fan_holds_test fan_pins_test fan_last_slot_test fan_histories_test fan_layout_test
        fan_threads_test fan_threads_lagging_test)
    add_test(NAME ${test} COMMAND poor_fan_tests ${test})
endforeach()

function(fan_compile_test case regex)
    add_library(poor_fan_compile_${case} OBJECT EXCLUDE_FROM_ALL poor_fan_compile_${case}.c)
    target_link_libraries(poor_fan_compile_${case} poor_base)
    add_test(NAME poor_fan_compile_${case} COMMAND ${CMAKE_COMMAND} --build . --target poor_fan_compile_${case} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set_tests_properties(poor_fan_compile_${case} PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
endfunction()

fan_compile_test(no_attach "(invalid use of undefined|incomplete definition of) type [^ ]*struct h_fan_meta_frame_fan")
fan_compile_test(conflicting_attach "redefinition of struct or union [^ ]*struct h_fan_meta_frame_fan|h_fan_meta_frame_fan.* has incompatible definitions")
fan_compile_test(wrong_fan "fan \\(fan\\) must be an unqualified frame_fan")
fan_compile_test(const_fan_member "fan \\(fan\\) must be an unqualified frame_fan")
fan_compile_test(array_refs "refs \\(refs\\) must be an unqualified pointer to struct poor_fan_ref")
fan_compile_test(const_refs "refs \\(refs\\) must be an unqualified pointer to struct poor_fan_ref")
fan_compile_test(wrong_published "published slot \\(published\\) must be an unqualified uint32_t")
fan_compile_test(shared_line "refs \\(refs\\) and published slot \\(published\\) share a cache line")
fan_compile_test(init_first_ref "refs \\(refs\\) must be a pointer to an array of struct poor_fan_ref")
fan_compile_test(init_wrong_type "refs \\(&counts\\) must be a pointer to an array of struct poor_fan_ref")
fan_compile_test(init_too_many "refs \\(&refs\\) has more items than POOR_FAN_MAX_SLOTS")
