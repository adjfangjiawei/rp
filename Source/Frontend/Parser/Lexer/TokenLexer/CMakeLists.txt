# Create library for token lexer component
add_library(LexerTokenParser
    ${CMAKE_CURRENT_SOURCE_DIR}/TokenLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TokenLexer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CommentHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CommentHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TemplateTokenHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TemplateTokenHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CoroutineTokenHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CoroutineTokenHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LambdaTokenHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LambdaTokenHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PreprocessorHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PreprocessorHandler.h
)

# Set include directories for this target
target_include_directories(LexerTokenParser
    PUBLIC
    ${RP_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(LexerTokenParser
    PUBLIC
    LexerToken
)
