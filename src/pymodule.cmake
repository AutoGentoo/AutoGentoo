function(setup_pymodule target)
    # Link the module to the python runtime
    target_include_directories(${target} PUBLIC ${Python_INCLUDE_DIRS})
    target_link_libraries(${target} PUBLIC Python3::Python)

    # Don't name the library libautogentoo_network
    # because python does not like the 'lib' prefix
    set_target_properties(
            ${target}
            PROPERTIES
            PREFIX ""
    )
endfunction()
