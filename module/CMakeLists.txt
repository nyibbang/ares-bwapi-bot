add_library(AresModule SHARED)

target_sources(AresModule
    PRIVATE
        Module.h
        Module.cpp
        dll.cpp
)

target_include_directories(
    AresModule
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    AresModule
    PRIVATE
        AresCore
        BWAPI::BWAPI
        quill::quill
)

install(TARGETS AresModule DESTINATION ".")
