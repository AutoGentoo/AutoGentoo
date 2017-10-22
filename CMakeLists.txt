cmake_minimum_required(VERSION 3.8)
project(Hacksaw)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER gcc)
include_directories(include)

set(SOURCE_FILES main.c include/package/package.h include/Hacksaw.h include/package/manifest.h include/tools/vector.h src/vector.c src/manifest.c include/portage/repository.h src/repository.c include/tools/regular_expression.h src/regular_expression.c include/tools/string_vector.h src/string_vector.c include/tools/config.h src/config.c)
add_executable(Hacksaw ${SOURCE_FILES})