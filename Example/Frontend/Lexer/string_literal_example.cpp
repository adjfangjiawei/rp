#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

using namespace rp::frontend;

void printToken(const Token& token) {
    std::cout << "Token: " << token.getText() << " (Kind: " << static_cast<int>(token.getKind()) << ")"
              << " at Line: " << token.getLine() << ", Column: " << token.getColumn() << std::endl;
}

int main() {
    auto diagEngine = std::make_shared<DiagnosticEngine>();
    Lexer lexer(diagEngine);

    // 测试各种字符串字面量
    const char* testCode = R"(
    // 基本字符串字面量
    "Hello, World!"
    ""  // 空字符串
    
    // 字符字面量
    'a'
    '\n'
    
    // 转义序列
    "First line\nSecond line"  // 换行
    "Tab\there"                // 制表符
    "Quote\"inside\"quote"     // 引号转义
    "Backslash: \\"           // 反斜杠转义
    "\x48\x65\x6C\x6C\x6F"    // 十六进制转义
    "\110\145\154\154\157"    // 八进制转义
    
    // Unicode字符
    "Unicode: \u0048\u0065\u006C\u006C\u006F"  // Unicode转义
    "Wide: \U0001F600"  // 宽Unicode字符(emoji)
    
    // 原始字符串(Raw strings)
    R"(This is a raw string
    No need to escape \n or \t
    Can span multiple lines)"

                           // 带分隔符的原始字符串
                           R"delim(This string has )delim inside)delim"

                           // 字符串连接
                           "Hello "
                           "World"  // 自动连接
                           "Multi-line "
                           "string "
                           "concatenation";

    lexer.setSource(testCode, strlen(testCode), "string_test.cpp");

    std::cout << "=== 字符串字面量测试 ===" << std::endl;

    // 读取并打印所有token
    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) {
            break;
        }
        // 只打印字符串相关的token
        if (token.getKind() == TokenKind::StringLiteral || token.getKind() == TokenKind::CharLiteral) {
            printToken(token);
            // 打印token的具体位置的上下文
            std::string context = lexer.getErrorContext(token.getLine(), token.getColumn());
            std::cout << "Context:\n" << context << "\n\n";
        }
    }

    // 测试错误情况
    const char* errorTestCode = R"(
    "Unterminated string
    'Unterminated char
    "Invalid escape \z"
    )";

    std::cout << "\n=== 错误处理测试 ===" << std::endl;
    lexer.setSource(errorTestCode, strlen(errorTestCode), "string_error_test.cpp");

    // 读取并打印错误token
    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) {
            break;
        }
        printToken(token);
    }

    return 0;
}
