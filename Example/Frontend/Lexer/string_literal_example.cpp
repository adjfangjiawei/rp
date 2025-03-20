#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
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

    // 从文件读取测试代码
    std::ifstream file("Example/Frontend/Lexer/test_input.txt");
    if (!file.is_open()) {
        std::cerr << "无法打开测试文件" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string testCodeStr = buffer.str();
    const char* testCode = testCodeStr.c_str();

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
        } else if (token.getKind() == TokenKind::Invalid) {
            // std::cout << "Error: " << token.getErrorMessage() << std::endl;
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
