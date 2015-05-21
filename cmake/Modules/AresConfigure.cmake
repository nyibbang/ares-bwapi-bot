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

set(CONFIG_MAJOR_VERSION ${${PROJECT_NAME}_MAJOR_VERSION})
set(CONFIG_MINOR_VERSION ${${PROJECT_NAME}_MINOR_VERSION})
set(CONFIG_PATCH_VERSION ${${PROJECT_NAME}_PATCH_VERSION})

set(CONFIG_DEBUG 0)
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CONFIG_DEBUG 1)
endif()

set(CONFIG_MSWINDOWS 0)
if (WIN32)
    set(CONFIG_MSWINDOWS 1)
endif()

message(STATUS "Creating 'config.h' from configuration settings")
configure_file(config.h.in config.h @ONLY)
include_directories(${PROJECT_BINARY_DIR})

unset(CONFIG_MAJOR_VERSION)
unset(CONFIG_MINOR_VERSION)
unset(CONFIG_PATCH_VERSION)
unset(CONFIG_DEBUG)
unset(CONFIG_MSWINDOWS)

