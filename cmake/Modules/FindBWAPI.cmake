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

# List of standard paths where BWAPI could be found
set(_PF86 "ProgramFiles(x86)")
set(BWAPI_PATHS
    $ENV{ProgramFiles}
    $ENV{${_PF86}}
    $ENV{ProgramW6432}
    $ENV{BWAPI_DIR}
    $ENV{BWAPIDIR}
    ${BWAPI_DIR}
    ${BWAPIDIR}
)

# Find BWAPI include directory
find_path(BWAPI_INCLUDE_DIR BWAPI.h PATHS ${BWAPI_PATHS} PATH_SUFFIXES "BWAPI/include")
set(BWAPI_INCLUDE_DIRS ${BWAPI_INCLUDE_DIR})

# Find BWAPI libraries (both debug and release)
set(BWAPI_POSTFIX_RELEASE "")
set(BWAPI_POSTFIX_DEBUG "d")
foreach(_config_type RELEASE DEBUG)
    find_library(BWAPI_LIBRARY_${_config_type} BWAPI${BWAPI_POSTFIX_${_config_type}}
                 PATHS ${BWAPI_PATHS} PATH_SUFFIXES "BWAPI/lib" "lib")
endforeach()
set(BWAPI_LIBRARY "debug;${BWAPI_LIBRARY_DEBUG};optimized;${BWAPI_LIBRARY_RELEASE}")
set(BWAPI_LIBRARIES ${BWAPI_LIBRARY})
unset(BWAPI_POSTFIX_RELEASE)
unset(BWAPI_POSTFIX_DEBUG)

unset(BWAPI_PATHS)

# Handle the QUIETLY and REQUIRED arguments and set BWAPI_FOUND to TRUE
# if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BWAPI DEFAULT_MSG BWAPI_LIBRARY BWAPI_INCLUDE_DIR)

mark_as_advanced(BWAPI_INCLUDE_DIR BWAPI_LIBRARY)

