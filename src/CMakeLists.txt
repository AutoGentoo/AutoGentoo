# Setup the Python development environment
set(Python3_FIND_VIRTUALENV "ONLY")
find_package(Python3 COMPONENTS Development)

# Add util libraries
add_subdirectory(hacksaw)

# Define all of the Python submodules
# This will compile all of the interface modules
include(pymodule.cmake)  # Python module macros

add_subdirectory(network)
add_subdirectory(cportage)