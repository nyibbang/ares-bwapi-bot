if(NOT MSVC)
    message(STATUS "Ignoring AresModule library because project is not "
                   "being built under MSVC.")
    return()
endif()

# Find BWAPI library
set(_postfix_RELEASE "")
set(_postfix_DEBUG "d")
set(BWAPI_PATHS
	"$ENV{ProgramFiles}"
	"$ENV{ProgramFiles(x86)}"
	"$ENV{ProgramW6432}"
	"$ENV{BWAPI_DIR}"
	"${BWAPI_DIR}"
)
foreach(_config_type RELEASE DEBUG)
	find_library(BWAPI_LIBRARY_${_config_type}
		BWAPI${CMAKE_${_config_type}_POSTFIX}
		PATHS ${BWAPI_PATHS}
		PATH_SUFFIXES "BWAPI/lib"
	)
endforeach()
unset(_postfix_RELEASE)
unset(_postfix_DEBUG)
set(BWAPI_LIBRARY "debug;${BWAPI_LIBRARY_DEBUG};optimized;${BWAPI_LIBRARY_RELEASE}")

find_path(BWAPI_INCLUDE_DIR
	BWAPI.h
	PATHS ${BWAPI_PATHS}
	PATH_SUFFIXES "BWAPI/include"
)

add_library(AresModule SHARED
    AresAIModule.h AresAIModule.cpp
    dll.cpp
)
target_include_directories(AresModule
	PUBLIC ${BWAPI_INCLUDE_DIR}
)
target_link_libraries(AresModule
    AresCore ${BWAPI_LIBRARY}
)

