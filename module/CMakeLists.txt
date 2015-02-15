if(NOT MSVC)
    message(STATUS "Ignoring AresModule library because project is not "
                   "being built under MSVC.")
    return()
endif()

add_library(AresModule SHARED
    AresAIModule.h AresAIModule.cpp
    dll.cpp
)
target_link_libraries(AresModule
    AresCore
)

