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
set(${PROJECT_NAME}_MINOR_VERSION 2)
set(${PROJECT_NAME}_PATCH_VERSION 1)

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

# Find starcraft dir
set(_PF86 "ProgramFiles(x86)")
find_path(STARCRAFT_DIR "StarCraft.exe"
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

# Install path
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${STARCRAFT_DIR}/bwapi-data/AI" CACHE
        PATH "default install path" FORCE)
endif()

################################################################################
# BWAPI library lookup
################################################################################
find_package(BWAPI)

################################################################################
# Configuration
################################################################################
include(AresConfigure)
include_directories(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

################################################################################
# Subdirectories
################################################################################
add_subdirectory(third_party)
add_subdirectory(core)
add_subdirectory(module)

