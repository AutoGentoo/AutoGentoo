# Setup the Python development environment
set(Python3_FIND_VIRTUALENV "ONLY")
find_package(Python3 COMPONENTS Development)

# This will compile all of the interface modules
include(pymodule.cmake)  # Python module macros

# Add util library
add_subdirectory(hacksaw)

# Define all of the Python submodules
add_subdirectory(network)
add_subdirectory(cportage)