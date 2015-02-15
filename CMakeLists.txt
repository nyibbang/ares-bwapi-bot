cmake_minimum_required(VERSION 2.8.11)

# Project declaration
project(AresBWAPIBot)
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 0)
set(${PROJECT_NAME}_PATCH_VERSION 0)

# Modules and packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")
include(EnableCXX11)
find_package(Threads REQUIRED)
enable_testing()
find_package(GTest)
find_package(GMock)

# Subdirectories
add_subdirectory(corelib)

