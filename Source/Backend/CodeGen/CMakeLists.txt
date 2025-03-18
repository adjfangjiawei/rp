target_sources(backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CodeGen.cpp
)

target_include_directories(backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
