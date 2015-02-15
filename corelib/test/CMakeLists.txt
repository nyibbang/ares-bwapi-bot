add_executable(AresCoreTest
    TestMain.cpp
    TestCore.cpp
)
add_test(NAME AresCoreTest COMMAND AresCoreTest)

target_include_directories(AresCoreTest
    PUBLIC
        ${gmock_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}/include
)
target_link_libraries(AresCoreTest
    AresCore
    gmock_main
)

