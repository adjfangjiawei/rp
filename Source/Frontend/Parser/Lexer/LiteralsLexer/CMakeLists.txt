# Create library for literals lexer components
add_library(LexerLiterals
    ${CMAKE_CURRENT_SOURCE_DIR}/CharacterLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CharacterLiteralLexer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StringLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StringLiteralLexer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer/Base/NumberLiteralBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer/Binary/BinaryLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer/Float/FloatLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer/Hex/HexLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer/Integer/IntegerLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer/Octal/OctalLiteralLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NumberLiteralLexer/Suffix/SuffixProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StringLiteralLexer/StringLiteralUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StringLiteralLexer/RawStringProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StringLiteralLexer/EscapeSequenceProcessor.cpp
)

# Set include directories for this target
target_include_directories(LexerLiterals
    PUBLIC
    ${RP_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(LexerLiterals
    PUBLIC
    LexerToken
    LexerUtils
)
