set(_PF86 "ProgramFiles(x86)")
set(BWAPI_PATHS
    $ENV{ProgramFiles}
    $ENV{${_PF86}}
    $ENV{ProgramW6432}
    $ENV{BWAPI_DIR}
    ${BWAPI_DIR}
)

# Find BWAPI include directory
find_path(
    BWAPI_INCLUDE_DIR BWAPI.h
    PATHS ${BWAPI_PATHS}
    PATH_SUFFIXES include
)
set(BWAPI_INCLUDE_DIRS ${BWAPI_INCLUDE_DIR})

# Find BWAPI libraries (both debug and release)
find_library(
    BWAPI_LIBRARY_DEBUG BWAPILIB
    PATHS ${BWAPI_DIR}
    PATH_SUFFIXES Debug
)
find_library(
    BWAPI_LIBRARY_RELEASE BWAPILIB
    PATHS ${BWAPI_DIR}
    PATH_SUFFIXES Release
)
set(BWAPI_LIBRARY ${BWAPI_LIBRARY_RELEASE})

add_library(BWAPI::BWAPI STATIC IMPORTED)

set_target_properties(
    BWAPI::BWAPI
    PROPERTIES
        IMPORTED_LOCATION_DEBUG ${BWAPI_LIBRARY_DEBUG}
        IMPORTED_LOCATION_RELEASE ${BWAPI_LIBRARY_RELEASE}
)

target_include_directories(
    BWAPI::BWAPI
    INTERFACE
        ${BWAPI_INCLUDE_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    BWAPI DEFAULT_MSG
    BWAPI_LIBRARY_DEBUG
    BWAPI_LIBRARY_RELEASE
    BWAPI_INCLUDE_DIR
)
mark_as_advanced(
    BWAPI_LIBRARY_DEBUG
    BWAPI_LIBRARY_RELEASE
    BWAPI_LIBRARY
    BWAPI_INCLUDE_DIR
    BWAPI_INCLUDE_DIRS
)
