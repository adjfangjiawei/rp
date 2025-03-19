# Create library for lexer utilities
add_library(LexerUtils
INTERFACE
)

# Set include directories for this target
target_include_directories(LexerUtils
    INTERFACE
    ${RP_INCLUDE_DIR}
)
