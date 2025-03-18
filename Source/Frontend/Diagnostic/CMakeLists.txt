add_library(Diagnostic Diagnostic.cpp)

target_include_directories(Diagnostic
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(frontend
    INTERFACE
    Diagnostic
)