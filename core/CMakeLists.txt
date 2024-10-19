add_library(AresCore STATIC)

target_sources(
    AresCore
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/config.h
    PRIVATE
        dummy.cpp
)

target_include_directories(
    AresCore
    PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

message(STATUS "Creating 'config.h' from configuration settings")
configure_file(config.h.in config.h @ONLY)
