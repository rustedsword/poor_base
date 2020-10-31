add_executable(test_poor_map poor_map_test.c )
target_link_libraries(test_poor_map poor_base)
add_test(test_poor_map test_poor_map)
