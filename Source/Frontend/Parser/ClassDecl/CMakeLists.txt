target_sources(frontend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ClassDeclParser.cpp
)

target_include_directories(frontend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
