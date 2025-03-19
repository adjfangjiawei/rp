add_subdirectory(ErrorRecovery)
add_subdirectory(LiteralsLexer)
add_subdirectory(MainLexer)
add_subdirectory(TokenLexer)
add_subdirectory(Token)
add_subdirectory(Unicode)
add_subdirectory(Trace)
add_subdirectory(Utils)
add_subdirectory(Tests)

# Create the main Lexer library target
add_library(Lexer INTERFACE)

# Set include directories for the target
target_include_directories(Lexer
    INTERFACE
    ${RP_INCLUDE_DIR}
)

# Link all Lexer components
target_link_libraries(Lexer
    INTERFACE
    MainLexer
    LexerLiterals
    LexerToken
    TokenLexer
    LexerErrorRecovery
    LexerTrace
    LexerUtils
    Diagnostic
    LexerUnicode
)

target_link_libraries(frontend INTERFACE Lexer)
