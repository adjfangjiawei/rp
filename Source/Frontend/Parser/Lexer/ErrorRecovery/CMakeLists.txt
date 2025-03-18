# Create library for error recovery component
add_library(LexerErrorRecovery
    ${CMAKE_CURRENT_SOURCE_DIR}/ErrorRecovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ErrorRecovery.h
)

# Set include directories for this target
target_include_directories(LexerErrorRecovery
    PUBLIC
    ${RP_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(LexerErrorRecovery
    PUBLIC
    LexerToken
)
