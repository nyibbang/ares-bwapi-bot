cmake_minimum_required(VERSION 3.28)
project(AresBWAPIBot VERSION 0.2.1)

# Debug libraries should be postfixed with a 'd'
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find starcraft dir
set(_PF86 "ProgramFiles(x86)")
find_path(STARCRAFT_ROOT "StarCraft.exe"
    PATHS
        $ENV{ProgramFiles}
        $ENV{${_PF86}}
        $ENV{ProgramW6432}
        $ENV{BWAPI_DIR}
        $ENV{BWAPIDIR}
        ${BWAPI_DIR}
        ${BWAPIDIR}
    PATH_SUFFIXES "StarCraft"
)

find_package(BWAPI REQUIRED)
find_package(quill REQUIRED)

add_subdirectory(core)
add_subdirectory(module)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
