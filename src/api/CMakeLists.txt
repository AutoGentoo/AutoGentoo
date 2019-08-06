find_program(PYTHON "python")

if (PYTHON)
    set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py")
    set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
    set(DEPS        "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/d_malloc.pyx"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/d_malloc.pxd"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/dynamic_binary.pyx"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/dynamic_binary.pxd"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/request.pyx"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/request.pxd"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/stream.pyx"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/stream.pxd"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/vector.pyx"
                    "${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/vector.pxd")

    set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")

    configure_file(${SETUP_PY_IN} ${SETUP_PY})

    add_custom_command(OUTPUT ${OUTPUT}
            COMMAND ${PYTHON} ${SETUP_PY} build_ext
            COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
            DEPENDS ${DEPS})

    add_custom_target(autogentoo_api ALL DEPENDS ${OUTPUT})


    install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install)")
endif()