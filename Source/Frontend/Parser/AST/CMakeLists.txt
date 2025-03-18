target_sources(frontend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TypeConversion.cpp
)

target_include_directories(frontend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
