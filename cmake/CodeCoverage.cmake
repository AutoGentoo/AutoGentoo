# Copyright (c) 2012 - 2015, Lars Bilke
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#
#
# 2012-01-31, Lars Bilke
# - Enable Code Coverage
#
# 2013-09-17, Joakim Söderberg
# - Added support for Clang.
# - Some additional usage instructions.
#
# USAGE:

# 0. (Mac only) If you use Xcode 5.1 make sure to patch geninfo as described here:
#      http://stackoverflow.com/a/22404544/80480
#
# 1. Copy this file into your cmake modules path.
#
# 2. Add the following line to your CMakeLists.txt:
#      INCLUDE(CodeCoverage)
#
# 3. Set compiler flags to turn off optimization and enable coverage:
#    SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#     SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#
# 3. Use the function SETUP_TARGET_FOR_COVERAGE to create a custom make target
#    which runs your test executable and produces a lcov code coverage report:
#    Example:
#     SETUP_TARGET_FOR_COVERAGE(
#                my_coverage_target  # Name for custom target.
#                test_driver         # Name of the test driver executable that runs the tests.
#                                    # NOTE! This should always have a ZERO as exit code
#                                    # otherwise the coverage generation will not complete.
#                coverage            # Name of output directory.
#                )
#
# 4. Build a Debug build:
#     cmake -DCMAKE_BUILD_TYPE=Debug ..
#     make
#     make my_coverage_target
#
#

# Check prereqs
FIND_PROGRAM( GCOV_PATH gcov )
FIND_PROGRAM( LCOV_PATH lcov )
FIND_PROGRAM( GENHTML_PATH genhtml )
FIND_PROGRAM( GCOVR_PATH gcovr PATHS ${CMAKE_SOURCE_DIR}/tests)

# Display an error when the target is called. If no error is found, this
# function will be overridden by the real one later in this file
FUNCTION(SETUP_TARGET_FOR_COVERAGE _targetname _testrunner _outputname)
    ADD_CUSTOM_TARGET(${_targetname}
            COMMAND echo "Coverage is not available: ${ERROR_MSG}"
            )
ENDFUNCTION()

IF(NOT GCOV_PATH)
    set(ERROR_MSG "gcov not found")
    return()
ENDIF()
MARK_AS_ADVANCED(GCOV_PATH)
IF(NOT LCOV_PATH)
    set(ERROR_MSG "lcov not found")
    return()
ENDIF()
MARK_AS_ADVANCED(LCOV_PATH)
IF(NOT GENHTML_PATH)
    set(ERROR_MSG "genhtml not found")
    return()
ENDIF()
MARK_AS_ADVANCED(GENHTML_PATH)
IF(NOT CMAKE_COMPILER_IS_GNUCXX)
    set(ERROR_MSG "Compiler is not gcc")
    return()
ENDIF()

SET(CMAKE_CXX_FLAGS_COVERAGE
        "-g -O0 --coverage -fprofile-arcs -ftest-coverage"
        CACHE STRING "Flags used by the C++ compiler during coverage builds."
        FORCE )
SET(CMAKE_C_FLAGS_COVERAGE
        "-g -O0 --coverage -fprofile-arcs -ftest-coverage"
        CACHE STRING "Flags used by the C compiler during coverage builds."
        FORCE )
SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE
        ""
        CACHE STRING "Flags used for linking binaries during coverage builds."
        FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
        ""
        CACHE STRING "Flags used by the shared libraries linker during coverage builds."
        FORCE )
MARK_AS_ADVANCED(
        CMAKE_CXX_FLAGS_COVERAGE
        CMAKE_C_FLAGS_COVERAGE
        CMAKE_EXE_LINKER_FLAGS_COVERAGE
        CMAKE_SHARED_LINKER_FLAGS_COVERAGE )

IF ( NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Coverage"))
    MESSAGE( WARNING "Code coverage results with an optimized (non-Debug) build may be misleading" )
ENDIF() # NOT CMAKE_BUILD_TYPE STREQUAL "Debug"


# Param _targetname     The name of new the custom make target
# Param _testrunner     The name of the target which runs the tests.
#                        MUST return ZERO always, even on errors.
#                        If not, no coverage report will be created!
# Param _outputname     lcov output is generated as _outputname.info
#                       HTML report is generated in _outputname/index.html
# Optional fifth parameter is passed as arguments to _testrunner
#   Pass them in list form, e.g.: "-j;2" for -j 2
FUNCTION(SETUP_TARGET_FOR_COVERAGE _targetname _testrunner _outputname)

    # Setup target
    ADD_CUSTOM_TARGET(${_targetname}

            # Cleanup lcov
            COMMAND ${LCOV_PATH} --directory . --zerocounters

            # Create baseline coverage data file
            COMMAND ${LCOV_PATH} -c -i -d . -o ${_outputname}.baseline.info -q

            # Run tests
            COMMAND ${_testrunner} ${ARGV3}

            # Capturing lcov counters and generating report
            COMMAND ${LCOV_PATH} --directory . --capture --output-file ${_outputname}.info -q
            # Combine the baseline and the test data
            COMMAND ${LCOV_PATH} -a ${_outputname}.info -a ${_outputname}.baseline.info -o ${_outputname}.info -q

            # Remove information about source files that are not part of
            # the test (system file, external libraries, etc)
            COMMAND ${LCOV_PATH} --remove ${_outputname}.info '/usr/*' '*/external/*' --output-file ${_outputname}.info -q

            # Generate the report
            COMMAND ${GENHTML_PATH} -o ${_outputname} ${_outputname}.info

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
            )

    # Show info where to find the report
    ADD_CUSTOM_COMMAND(TARGET ${_targetname} POST_BUILD
            COMMAND ;
            COMMENT "Open ./${_outputname}/index.html in your browser to view the coverage report."
            )

ENDFUNCTION() # SETUP_TARGET_FOR_COVERAGE

# Param _targetname     The name of new the custom make target
# Param _testrunner     The name of the target which runs the tests
# Param _outputname     cobertura output is generated as _outputname.xml
# Optional fourth parameter is passed as arguments to _testrunner
#   Pass them in list form, e.g.: "-j;2" for -j 2
FUNCTION(SETUP_TARGET_FOR_COVERAGE_COBERTURA _targetname _testrunner _outputname)

    IF(NOT PYTHON_EXECUTABLE)
        MESSAGE(FATAL_ERROR "Python not found! Aborting...")
    ENDIF() # NOT PYTHON_EXECUTABLE

    IF(NOT GCOVR_PATH)
        MESSAGE(FATAL_ERROR "gcovr not found! Aborting...")
    ENDIF() # NOT GCOVR_PATH
    MARK_AS_ADVANCED(GCOVR_PATH)

    ADD_CUSTOM_TARGET(${_targetname}

            # Run tests
            ${_testrunner} ${ARGV3}

            # Running gcovr
            COMMAND ${GCOVR_PATH} -x -r ${CMAKE_SOURCE_DIR} -e '${CMAKE_SOURCE_DIR}/tests/'  -o ${_outputname}.xml
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running gcovr to produce Cobertura code coverage report."
            )

    # Show info where to find the report
    ADD_CUSTOM_COMMAND(TARGET ${_targetname} POST_BUILD
            COMMAND ;
            COMMENT "Cobertura code coverage report saved in ${_outputname}.xml."
            )

ENDFUNCTION() # SETUP_TARGET_FOR_COVERAGE_COBERTURA
