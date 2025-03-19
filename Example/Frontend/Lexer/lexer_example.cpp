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
    // 创建诊断引擎
    DiagnosticEngine diagEngine;

    // 创建词法分析器
    Lexer lexer(&diagEngine);

    // 测试代码
    const char* testCode = R"(
int main() {
    int x = 42;
    float y = 3.14;
    char* str = "Hello World";
    return 0;
}
)";

    // 设置源代码
    lexer.setSource(testCode, strlen(testCode), "test.cpp");

    std::cout << "=== 基本词法分析测试 ===" << std::endl;

    // 测试nextToken()
    std::cout << "\n1. 测试nextToken():" << std::endl;
    for (int i = 0; i < 5; ++i) {
        Token token = lexer.nextToken();
        printToken(token);
    }

    // 测试peekToken()
    std::cout << "\n2. 测试peekToken():" << std::endl;
    Token peeked = lexer.peekToken();
    std::cout << "Peeked token: " << peeked.getText() << std::endl;

    // 测试peekToken(n)
    std::cout << "\n3. 测试peekToken(2):" << std::endl;
    Token peeked2 = lexer.peekToken(2);
    std::cout << "Peeked token (2 ahead): " << peeked2.getText() << std::endl;

    // 测试ungetToken
    std::cout << "\n4. 测试ungetToken():" << std::endl;
    Token token = lexer.nextToken();
    printToken(token);
    lexer.ungetToken(token);
    Token tokenAgain = lexer.nextToken();
    std::cout << "Token after unget: " << tokenAgain.getText() << std::endl;

    // 测试位置信息
    std::cout << "\n5. 测试位置信息:" << std::endl;
    auto [line, col] = lexer.getCurrentPosition();
    std::cout << "Current position - Line: " << line << ", Column: " << col << std::endl;

    // 测试错误上下文
    std::cout << "\n6. 测试错误上下文:" << std::endl;
    std::string context = lexer.getErrorContext(line, col);
    std::cout << "Error context:\n" << context << std::endl;

    return 0;
}
