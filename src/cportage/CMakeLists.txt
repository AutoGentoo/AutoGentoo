find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(language_PARSER
        depend_bison.y
        depend_flex.l
        make_conf_bison.y
        make_conf_flex.l
        )

flex_target(FLEX_DEPEND depend_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/depend.flex.c)
bison_target(LANG_DEPEND
        depend_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/depend.tab.c)

flex_target(FLEX_MAKE_CONF make_conf_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/make_conf.flex.c)
bison_target(LANG_MAKE_CONF
        make_conf_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/make_conf.tab.c)

add_flex_bison_dependency(FLEX_DEPEND LANG_DEPEND)
add_flex_bison_dependency(FLEX_MAKE_CONF LANG_MAKE_CONF)

set(language_BIN
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_FLEX_DEPEND_OUTPUTS}
        ${BISON_LANG_MAKE_CONF_OUTPUTS}
        ${FLEX_FLEX_MAKE_CONF_OUTPUTS})

set(language_SOURCES
        ${language_BIN}
        ${language_PARSER}
        ${language_HEADERS})

add_library(autogentoo_cportage SHARED
        ${language_SOURCES}
        portage.h module.c use.c use.h atom.c
        ../python_util.h atom.h dependency.c dependency.h portage.c)

setup_pymodule(autogentoo_cportage)
target_include_directories(autogentoo_cportage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(autogentoo_cportage PUBLIC hacksaw)

add_custom_target(autogentoo_cportage_test
        COMMAND ${LD_PRELOAD} ${CMAKE_CURRENT_SOURCE_DIR}/test.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_cportage.so
)
