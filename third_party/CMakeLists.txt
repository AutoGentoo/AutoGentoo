# Disable the cmocka tests
set(CMAKE_DISABLED_TESTS
        ${CMAKE_DISABLED_TESTS}
        simple_test
        allocate_module_test
        assert_macro_test
        assert_module_test
        waiter_test_wrap
        test_uptime)
add_subdirectory(cmocka EXCLUDE_FROM_ALL)
add_subdirectory(neoast EXCLUDE_FROM_ALL)