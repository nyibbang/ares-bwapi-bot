# Add the executable
add_executable(AresCoreTest 
    TestMain.cpp
    TestCore.cpp
)
target_link_libraries(AresCoreTest AresCore)

# Linked to BOOST
include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(AresCoreTest
    ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME AresCoreTest COMMAND AresCoreTest)

