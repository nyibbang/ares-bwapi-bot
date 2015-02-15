# If either GTest or GMock could not be found, drop the test
if (NOT GTEST_FOUND OR NOT GMOCK_FOUND)
    message(STATUS "Ignoring ${CMAKE_CURRENT_LIST_DIR} folder because either "
                   "GTest or GMock could not be found on your system.")
    return()
endif()

add_executable(AresCoreTest
    TestMain.cpp
    TestCore.cpp
)
add_test(NAME AresCoreTest COMMAND AresCoreTest)

target_include_directories(AresCoreTest
    PUBLIC
        ${GTEST_INCLUDE_DIRS}
        ${GMOCK_INCLUDE_DIRS}
)
target_link_libraries(AresCoreTest
    AresCore
    ${GTEST_BOTH_LIBRARIES}
    ${GMOCK_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

