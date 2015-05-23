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
# CoreTest executable
################################################################################
add_executable(AresCoreTest
    TestMain.cpp
    TestCore.cpp
)
add_test(NAME AresCoreTest COMMAND AresCoreTest)

target_compile_definitions(AresCoreTest
    PUBLIC
        ARES_DISABLE_TRACE
)
target_include_directories(AresCoreTest
    PUBLIC
        ${gmock_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}/include
)
target_link_libraries(AresCoreTest
    AresCore
    gmock_main
)

