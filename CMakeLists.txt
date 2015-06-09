#------------------------------------------------------------------------------
# This file is part of the CMake build system for Infector++ (Infectorpp)
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

################################################################################
# Set up the basic build environment
################################################################################
# Just debug / release since that's all that's all we need
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(INFECTORPP_PROJECT "Infector++")
project(${INFECTORPP_PROJECT})

include_directories(include)

set(INFECTORPP_SOURCE
    include/Infectorpp/Infector_details.hpp
    include/Infectorpp/Infector_multi.hpp
    include/Infectorpp/Infector_private.hpp
    include/Infectorpp/Infector_single.hpp
    include/Infectorpp/InfectorContainer.hpp
    include/Infectorpp/InfectorExceptions.hpp
    include/Infectorpp/InfectorExport.hpp
    include/Infectorpp/InfectorHelpers.hpp
)
source_group(infectorcpp FILES ${INFECTORPP_SOURCE})

set(GTEST_SOURCE
    tests/gtest/gtest/gtest-death-test.h
    tests/gtest/gtest/gtest-message.h
    tests/gtest/gtest/gtest-param-test.h
    tests/gtest/gtest/gtest-printers.h
    tests/gtest/gtest/gtest-spi.h
    tests/gtest/gtest/gtest-test-part.h
    tests/gtest/gtest/gtest-typed-test.h
    tests/gtest/gtest/gtest.h
    tests/gtest/gtest/gtest_pred_impl.h
    tests/gtest/gtest/gtest_prod.h
)
source_group(gtest FILES ${GTEST_SOURCE})

set(GTEST_INTERNAL_SOURCE
    tests/gtest/gtest/internal/gtest-death-test-internal.h
    tests/gtest/gtest/internal/gtest-filepath.h
    tests/gtest/gtest/internal/gtest-internal.h
    tests/gtest/gtest/internal/gtest-linked_ptr.h
    tests/gtest/gtest/internal/gtest-param-util-generated.h
    tests/gtest/gtest/internal/gtest-param-util.h
    tests/gtest/gtest/internal/gtest-port.h
    tests/gtest/gtest/internal/gtest-string.h
    tests/gtest/gtest/internal/gtest-tuple.h
    tests/gtest/gtest/internal/gtest-type-util.h
)
source_group(gtest\\internal FILES ${GTEST_INTERNAL_SOURCE})

set(INFECTORPP_TEST_FILES
    # Test files
    tests/UnitTest.cpp
)
source_group(tests FILES ${INFECTOR_TEST_FILES})

include_directories(tests/gtest)

add_executable(${INFECTORPP_PROJECT}
    ${INFECTORPP_SOURCE}
    ${GTEST_SOURCE}
    ${GTEST_INTERNAL_SOURCE}
    ${INFECTORPP_TEST_FILES}
)
