BuildParser(depend_parser depend.y)
BuildParser(requireduse_parser requireduse.y)

add_compile_options(-g -fno-omit-frame-pointer)

PythonModule(autogentoo_cportage
        SOURCES
        portage.h module.c use.c use.h atom.c
        ../python_util.h atom.h dependency.c
        dependency.h portage.c ebuild.c ebuild.h module.h
        language.h package.c package.h

        ${depend_parser_OUTPUT}
        ${requireduse_parser_OUTPUT}

        INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}
        LINK_LIBRARIES autogentoo_hacksaw neoast
        )
