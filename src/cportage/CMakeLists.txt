find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(language_PARSER
        depend_bison.y
        depend_flex.l
        language.c
        )

flex_target(FLEX_DEPEND depend_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/depend.flex.c)
bison_target(LANG_DEPEND
        depend_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/depend.tab.c)

add_flex_bison_dependency(FLEX_DEPEND LANG_DEPEND)

add_compile_options(-g -fno-omit-frame-pointer)

set(language_BIN
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_FLEX_DEPEND_OUTPUTS})

set(language_SOURCES
        ${language_BIN}
        ${language_PARSER}
        ${language_HEADERS})

PythonModule(autogentoo_cportage
        SOURCES ${language_SOURCES}
        portage.h module.c use.c use.h atom.c
        ../python_util.h atom.h dependency.c
        dependency.h portage.c ebuild.c ebuild.h module.h

        INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        LINK_LIBRARIES hacksaw
        )
