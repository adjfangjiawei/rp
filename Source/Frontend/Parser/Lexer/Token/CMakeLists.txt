set(TOKEN_HEADERS
    Token.h
    TokenKind.h
    TypeTokenKind.h
    TokenCategory.h
    TokenBase.h
    TokenUtils.h
)

set(TOKEN_SOURCES
    TokenKind.cpp
    TokenUtils.cpp
)

# 改为STATIC库，因为现在有源文件实现
add_library(LexerToken STATIC
    ${TOKEN_HEADERS}
    ${TOKEN_SOURCES}
)

target_include_directories(LexerToken 
    PUBLIC 
    ${RP_INCLUDE_DIR}
)

# 添加测试目标
if(BUILD_TESTING)
    add_executable(TokenTests
        Tests/TokenTests.cpp
    )
    
    target_link_libraries(TokenTests
        PRIVATE
        LexerToken
        GTest::gtest
        GTest::gtest_main
    )
    
    add_test(NAME TokenTests COMMAND TokenTests)
endif()
