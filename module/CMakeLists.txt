# AresBWAPIBot : A bot for SC:BW using BWAPI library;
# Copyright (C) 2015 Vincent PALANCHER; Florian LEMEASLE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA

################################################################################
# Preconditions:
#  - Compiling with MSVC (under Windows)
################################################################################
if(NOT MSVC)
    message(STATUS "Ignoring AresModule library because project is not "
                   "being built under MSVC.")
    return()
endif()

################################################################################
# BWAPI library lookup
################################################################################
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

################################################################################
# Module library
################################################################################
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

