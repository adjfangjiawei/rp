add_library(LexerToken
    TokenBase.cpp
    TokenKind.cpp
    TokenUtils.cpp
)

target_include_directories(LexerToken
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/TokenDefs
)

# 添加测试
if(BUILD_TESTING)
    add_subdirectory(Tests)
endif()
