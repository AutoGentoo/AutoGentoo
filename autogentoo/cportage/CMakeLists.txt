find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(depend_PARSER
        depend_bison.y
        depend_flex.l
        )

set(requireduse_PARSER
        requireduse_bison.y
        requireduse_flex.l
        )

flex_target(FLEX_DEPEND
        depend_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/depend.flex.c)
bison_target(LANG_DEPEND
        depend_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/depend.tab.c)

flex_target(FLEX_REQUIREDUSE
        requireduse_flex.l
        ${CMAKE_CURRENT_BINARY_DIR}/requireduse.flex.c)
bison_target(LANG_REQUIREDUSE
        requireduse_bison.y
        ${CMAKE_CURRENT_BINARY_DIR}/requireduse.tab.c)

add_flex_bison_dependency(FLEX_DEPEND LANG_DEPEND)
add_flex_bison_dependency(FLEX_REQUIREDUSE LANG_REQUIREDUSE)

add_compile_options(-g -fno-omit-frame-pointer)

set(depend_SOURCES
        ${depend_PARSER}
        ${BISON_LANG_DEPEND_OUTPUTS}
        ${FLEX_FLEX_DEPEND_OUTPUTS}
        )
set(requireduse_SOURCES
        ${requireduse_PARSER}
        ${BISON_LANG_REQUIREDUSE_OUTPUTS}
        ${FLEX_FLEX_REQUIREDUSE_OUTPUTS}
        )


PythonModule(autogentoo_cportage
        SOURCES
        portage.h module.c use.c use.h atom.c
        ../python_util.h atom.h dependency.c
        dependency.h portage.c ebuild.c ebuild.h module.h
        language.c package.c package.h

        ${depend_SOURCES}
        ${requireduse_SOURCES}

        INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        LINK_LIBRARIES autogentoo_hacksaw
        )
