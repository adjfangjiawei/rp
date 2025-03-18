# Create library for lexer utilities
add_library(LexerUtils
    ${CMAKE_CURRENT_SOURCE_DIR}/Unicode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Unicode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/UnicodeEscape.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UnicodeEscape.h
)

# Set include directories for this target
target_include_directories(LexerUtils
    PUBLIC
    ${RP_INCLUDE_DIR}
)
