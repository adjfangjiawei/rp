#include <cstring>
#include <iostream>
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

    // 测试各种数字字面量
    const char* testCode = R"(
    // 整数字面量
    42          // 十进制整数
    0x2A       // 十六进制整数
    052        // 八进制整数
    0b101010   // 二进制整数
    
    // 浮点数字面量
    3.14159    // 基本浮点数
    .123       // 省略整数部分
    42.        // 省略小数部分
    1e10       // 科学计数法
    1.23e-4    // 带小数的科学计数法
    
    // 带后缀的字面量
    42u        // 无符号整数
    42l        // 长整数
    42ul       // 无符号长整数
    3.14f      // float类型
    3.14l      // long double类型
    
    // 特殊情况
    0x1.2p3    // 十六进制浮点数
    1'000'000  // 带分隔符的数字
    )";

    lexer.setSource(testCode, strlen(testCode), "number_test.cpp");

    std::cout << "=== 数字字面量测试 ===" << std::endl;

    // 读取并打印所有token
    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) {
            break;
        }
        // 只打印数字相关的token
        if (token.getKind() == TokenKind::NumberLiteral) {
            printToken(token);
            // 打印token的具体位置的上下文
            std::string context = lexer.getErrorContext(token.getLine(), token.getColumn());
            std::cout << "Context:\n" << context << "\n\n";
        }
    }

    return 0;
}
