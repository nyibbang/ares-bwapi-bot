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
# Project
################################################################################
project(AresBWAPIBot)
cmake_minimum_required(VERSION 2.8.11)
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_VERSION 0)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Modules and packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")
include(EnableCXX11)
enable_testing()

# Debug libraries should be postfixed with a 'd'
set(CMAKE_DEBUG_POSTFIX "d")

################################################################################
# BWAPI library lookup if we are on MS Windows
################################################################################
if (WIN32)
    set(_BWAPI_POSTFIX_RELEASE "")
    set(_BWAPI_POSTFIX_DEBUG "d")
    set(BWAPI_PATHS
        "$ENV{ProgramFiles}"
        "$ENV{ProgramFiles(x86)}"
        "$ENV{ProgramW6432}"
        "$ENV{BWAPI_DIR}"
        "${BWAPI_DIR}"
    )
    foreach(_config_type RELEASE DEBUG)
        find_library(BWAPI_LIBRARY_${_config_type}
            BWAPI${_BWAPI_POSTFIX_${_config_type}}
            PATHS ${BWAPI_PATHS}
            PATH_SUFFIXES "BWAPI/lib" "lib"
        )
    endforeach()
    unset(_BWAPI_POSTFIX_RELEASE)
    unset(_BWAPI_POSTFIX_DEBUG)
    set(BWAPI_LIBRARY "debug;${BWAPI_LIBRARY_DEBUG};optimized;${BWAPI_LIBRARY_RELEASE}")

    find_path(BWAPI_INCLUDE_DIR
        BWAPI.h
        PATHS ${BWAPI_PATHS}
        PATH_SUFFIXES "BWAPI/include"
    )
endif()

################################################################################
# Subdirectories
################################################################################
add_subdirectory(third_party)
add_subdirectory(corelib)
add_subdirectory(module)

