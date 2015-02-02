cmake_minimum_required(VERSION 2.8.11)

# Project declaration
project(AresBWAPIBot)
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 0)
set(${PROJECT_NAME}_PATCH_VERSION 0)

# Using C++11
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++11 COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag(-std=c++0x COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    add_definitions(-std=c++11)
elseif(COMPILER_SUPPORTS_CXX0X)
    add_definitions(-std=c++0x)
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Looking for packages
find_package(Threads REQUIRED)

# Testing
enable_testing()
find_package(GTest REQUIRED)

# Core library declaration
add_library(AresCore STATIC
    Core.h Core.cpp
)
include_directories(.)

# Addition of the test folder
add_subdirectory(test)

