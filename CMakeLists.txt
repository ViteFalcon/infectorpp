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
	# Test files
	tests/UnitTest.cpp
)

add_executable(${INFECTORPP_PROJECT} ${INFECTORPP_SOURCE})
