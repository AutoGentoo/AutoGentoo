function(setup_pymodule target)
    # Link the module to the python runtime
    target_include_directories(${target} PUBLIC ${Python_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src/)
    target_link_libraries(${target} PUBLIC Python3::Python hacksaw)

    # Don't name the library libautogentoo_network
    # because python does not like the 'lib' prefix
    set_target_properties(
            ${target}
            PROPERTIES
            PREFIX ""
    )

    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:${target}> "${CMAKE_CURRENT_SOURCE_DIR}/${target}.so"
            BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/${target}.so"
            )

endfunction()
