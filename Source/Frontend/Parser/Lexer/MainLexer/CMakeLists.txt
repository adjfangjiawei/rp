
add_library(MainLexer
    LexerBase.cpp
    LexerTokenization.cpp
    LexerWhitespace.cpp
    LexerPosition.cpp
    Lexer.h
    KeywordManager.cpp
    KeywordManager.h
    Scanner.cpp
    Scanner.h
    BaseScanner.cpp
    BaseScanner.h
    UTF8Scanner.cpp
    UTF8Scanner.h
    IdentifierScanner.cpp
    IdentifierScanner.h
    OperatorScanner.cpp
    OperatorScanner.h
)

target_link_libraries(MainLexer
    PRIVATE
    LexerToken
    LexerLiterals
    LexerUtils
)

target_include_directories(MainLexer
    PUBLIC
     ${RP_INCLUDE_DIR}
)
