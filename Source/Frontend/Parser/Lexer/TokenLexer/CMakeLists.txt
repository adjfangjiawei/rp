# TokenLexer库
add_library(TokenLexer
    TokenLexer.cpp
    TokenLexer.h
    Preprocess/PreprocessorHandler.cpp
    Preprocess/PreprocessorHandler.h
    Preprocess/MacroExpander.cpp
    Preprocess/MacroExpander.h
    Preprocess/ConstantEvaluator.cpp
    Preprocess/ConstantEvaluator.h
    Preprocess/CacheStrategy.cpp
    Preprocess/CacheStrategy.h
    Preprocess/PreprocessorCache.cpp
    Preprocess/PreprocessorCache.h
    Comment/CommentHandler.cpp
    Comment/CommentHandler.h
    Coroutine/CoroutineTokenHandler.cpp
    Coroutine/CoroutineTokenHandler.h
    lambda/LambdaTokenHandler.cpp
    lambda/LambdaTokenHandler.h
    Template/TemplateTokenHandler.cpp
    Template/TemplateTokenHandler.h
    Serializer/TokenSerializer.cpp
    Serializer/TokenSerializer.h
)

# 查找并链接 cereal 库
find_package(cereal REQUIRED)

target_link_libraries(TokenLexer
    PRIVATE
        LexerToken
        Diagnostic
        cereal::cereal
)

target_include_directories(TokenLexer
PUBLIC
    ${RP_INCLUDE_DIR}
)

# 添加测试
if(BUILD_TESTING)
    add_executable(TokenLexerTests
        ../Tests/PreprocessorTest.cpp
        ../Tests/PreprocessorTest.h
    )

    target_link_libraries(TokenLexerTests
        PRIVATE
            TokenLexer
            gtest
            gtest_main
    )

    add_test(NAME TokenLexerTests COMMAND TokenLexerTests)
endif()
