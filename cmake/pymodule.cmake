function(LinkPythonLib target)
    set(options )
    set(oneValueArgs )
    set(multiValueArgs COMPILE_OPTIONS LINK_LIBRARIES LINK_OPTIONS INCLUDE_DIRECTORIES DEPEND)
    cmake_parse_arguments(PYTHON_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Link the module to the python runtime
    target_include_directories(${target} PUBLIC
            ${Python_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/autogentoo/
            ${PYTHON_LIB_INCLUDE_DIRECTORIES}
    )
    target_compile_options(${target} PUBLIC ${PYTHON_LIB_COMPILE_OPTIONS})
    target_link_libraries(${target} PUBLIC
            Python3::Python
            ${PYTHON_LIB_LINK_LIBRARIES})
    target_link_options(${target} PUBLIC ${LINK_OPTIONS})

    if ("${PYTHON_LIB_DEPEND}" STREQUAL "")
    else()
        add_dependencies(${target} ${PYTHON_LIB_DEPEND})
    endif()
endfunction()

function(PythonModule target)
    set(options )
    set(oneValueArgs )
    set(multiValueArgs SOURCES COMPILE_OPTIONS LINK_LIBRARIES LINK_OPTIONS INCLUDE_DIRECTORIES DEPEND)
    cmake_parse_arguments(PYTHON_MODULE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    add_library(${target} SHARED ${PYTHON_MODULE_SOURCES})

    LinkPythonLib(${target}
            COMPILE_OPTIONS ${PYTHON_MODULE_COMPILE_OPTIONS}
            LINK_LIBRARIES ${PYTHON_MODULE_LINK_LIBRARIES}
            LINK_OPTIONS ${PYTHON_MODULE_LINK_OPTIONS}
            INCLUDE_DIRECTORIES ${PYTHON_MODULE_INCLUDE_DIRECTORIES}
            DEPEND ${PYTHON_MODULE_DEPEND}
            )

    # Don't name the library libautogentoo_network
    # because python does not like the 'lib' prefix
    set_target_properties(
            ${target}
            PROPERTIES
            PREFIX ""
    )

    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${target}.so"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "$<TARGET_FILE:${target}>" "${CMAKE_CURRENT_SOURCE_DIR}/${target}.so"
            DEPENDS "$<TARGET_FILE:${target}>"
    )
endfunction()
