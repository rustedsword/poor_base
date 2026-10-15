add_executable(poor_rbtree_tests poor_rbtree_tests.c)
target_link_libraries(poor_rbtree_tests poor_base)
target_compile_options(poor_rbtree_tests PRIVATE -Wall -Wextra -Wshadow -Werror -pedantic-errors -UNDEBUG)

foreach(test IN ITEMS
        rbtree_init_test rbtree_insert_test rbtree_insert_order_test rbtree_remove_test rbtree_remove_order_test
        rbtree_find_test rbtree_first_last_test rbtree_foreach_test rbtree_foreach_safe_test rbtree_foreach_macro_name_test
        rbtree_single_eval_test rbtree_random_test rbtree_member_test rbtree_overaligned_test rbtree_declare_test)
    add_test(NAME ${test} COMMAND poor_rbtree_tests ${test})
endforeach()

# GCC -O1 calls the comparator without inlining it, so -Wmaybe-uninitialized sees what keys pass to it
add_library(poor_rbtree_tests_o1 OBJECT poor_rbtree_tests.c)
target_link_libraries(poor_rbtree_tests_o1 poor_base)
target_compile_options(poor_rbtree_tests_o1 PRIVATE -O1 -Wall -Wextra -Wshadow -Werror -pedantic-errors -UNDEBUG)

add_executable(rbtree_shared_tests rbtree_shared_tests.c rbtree_shared.c)
target_link_libraries(rbtree_shared_tests poor_base)
target_compile_options(rbtree_shared_tests PRIVATE -Wall -Wextra -Wshadow -Werror -pedantic-errors -UNDEBUG
    $<$<C_COMPILER_ID:Clang>:-Wno-c2y-extensions>)
add_test(NAME rbtree_shared_test COMMAND rbtree_shared_tests)

function(rbtree_compile_test name regex)
    add_library(${name} OBJECT EXCLUDE_FROM_ALL ${name}.c)
    target_link_libraries(${name} poor_base)
    add_test(NAME ${name} COMMAND ${CMAKE_COMMAND} --build . --target ${name} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set_tests_properties(${name} PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
endfunction()

set(tree_is_const "tree_is_const[^ ]* (is negative|declared as an array with a negative size)")
set(entry_is_const "entry_is_const[^ ]* (is negative|declared as an array with a negative size)")
set(wrong_type "not compatible with any (generic )?association")

rbtree_compile_test(poor_rbtree_define_compile_wrong_member "tree member \\(id\\) must be an unqualified struct poor_rbtree_node")
rbtree_compile_test(poor_rbtree_define_compile_const_member "tree member \\(node\\) must be an unqualified struct poor_rbtree_node")
rbtree_compile_test(poor_rbtree_define_compile_bool_cmp "tree comparator \\(item_less\\) must be int")
rbtree_compile_test(poor_rbtree_define_compile_mutable_cmp "tree comparator \\(item_cmp\\) must be int")
rbtree_compile_test(poor_rbtree_define_compile_block_scope
    "invalid storage class for function [^ ]*h_rbtree_wrap_item_tree|function definition is not allowed here")
rbtree_compile_test(poor_rbtree_declare_compile_no_define "type [^ ]*struct h_rbtree_meta_node_tree")
rbtree_compile_test(poor_rbtree_init_compile_const_tree "${tree_is_const}")
rbtree_compile_test(poor_rbtree_insert_compile_const_tree "${tree_is_const}")
rbtree_compile_test(poor_rbtree_insert_compile_const_entry "${entry_is_const}")
rbtree_compile_test(poor_rbtree_insert_compile_wrong_type "${wrong_type}")
rbtree_compile_test(poor_rbtree_remove_compile_const_tree "${tree_is_const}")
rbtree_compile_test(poor_rbtree_remove_compile_const_entry "${entry_is_const}")
rbtree_compile_test(poor_rbtree_find_compile_wrong_type "${wrong_type}")
