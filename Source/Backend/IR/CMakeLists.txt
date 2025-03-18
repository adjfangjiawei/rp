target_sources(backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/IR.cpp
)

target_include_directories(backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
