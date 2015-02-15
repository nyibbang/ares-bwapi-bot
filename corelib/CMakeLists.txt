# Core library declaration
add_library(AresCore STATIC
    Core.h Core.cpp
)
target_include_directories(AresCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Addition of the test folder
add_subdirectory(test)

