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
# CoreTest executable
################################################################################
add_executable(AresCoreLogTest
    TestMain.cpp
)
add_test(NAME AresCoreLogTest COMMAND AresCoreLogTest)

if(CMAKE_USE_PTHREADS_INIT)
    get_target_property(_old_flags AresCoreLogTest LINK_FLAGS)
    set(_new_flags "-Wl,--no-as-needed")
    if(_old_flags)
        set(_new_flags "${_old_flags} ${_new_flags}")
    endif()
    set_target_properties(AresCoreLogTest PROPERTIES LINK_FLAGS "${_new_flags}")
    unset(_new_flags)
endif()
target_include_directories(AresCoreLogTest
    PUBLIC
        ${gmock_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
)
target_link_libraries(AresCoreLogTest
    AresCoreLog
    gmock_main
    ${CMAKE_THREAD_LIBS_INIT}
)

