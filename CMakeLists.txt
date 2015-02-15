cmake_minimum_required(VERSION 2.8.11)

# Project declaration
project(AresBWAPIBot)
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 0)
set(${PROJECT_NAME}_PATCH_VERSION 0)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Modules and packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")
include(EnableCXX11)
enable_testing()

# Subdirectories
add_subdirectory(third_party)
add_subdirectory(corelib)
add_subdirectory(module)

