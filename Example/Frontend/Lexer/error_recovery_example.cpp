#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

using namespace rp::frontend;

class TestDiagnosticEngine : public DiagnosticEngine {
  public:
    void report(const std::string& message, size_t line, size_t column) {
        std::cout << "Error at Line " << line << ", Column " << column << ": " << message << std::endl;
    }
};

void printToken(const Token& token, const Lexer& lexer) {
    std::cout << std::left << std::setw(20) << token.getText() << " | Kind: " << std::setw(5)
              << static_cast<int>(token.getKind()) << " | Line: " << std::setw(4) << token.getLine()
              << " | Column: " << std::setw(4) << token.getColumn() << std::endl;

    // 打印错误上下文
    std::string context = lexer.getErrorContext(token.getLine(), token.getColumn());
    std::cout << "Context:\n" << context << "\n" << std::endl;
}

void runTest(const char* testCode, const char* testName, Lexer& lexer) {
    std::cout << "\n=== " << testName << " ===" << std::endl;
    lexer.setSource(testCode, strlen(testCode), "error_test.cpp");

    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) {
            break;
        }
        printToken(token, lexer);
    }
}

int main() {
    TestDiagnosticEngine diagEngine;
    Lexer lexer(&diagEngine);

    // 1. 未终止的字符串和字符测试
    const char* unterminatedTest = R"(
    // 未终止的字符串
    "This string never ends
    
    // 未终止的字符
    'a
    
    // 正常的token
    int x = 42;
    )";
    runTest(unterminatedTest, "未终止的字符串和字符测试", lexer);

    // 2. 非法字符测试
    const char* invalidCharTest = R"(
    // 非法字符
    int @ var = 42;
    float # = 3.14;
    
    // 非法标识符
    int 123abc = 456;
    )";
    runTest(invalidCharTest, "非法字符测试", lexer);

    // 3. 非法转义序列测试
    const char* invalidEscapeTest = R"(
    // 非法转义序列
    "Invalid escape \z"
    "Another invalid \u123"  // 不完整的Unicode转义
    'Invalid \x'  // 不完整的十六进制转义
    )";
    runTest(invalidEscapeTest, "非法转义序列测试", lexer);

    // 4. 数字字面量错误测试
    const char* numberErrorTest = R"(
    // 非法数字字面量
    123.456.789
    0x123G
    0b102
    08
    )";
    runTest(numberErrorTest, "数字字面量错误测试", lexer);

    // 5. 注释错误测试
    const char* commentErrorTest = R"(
    // 未终止的多行注释
    /* This comment never ends
    
    // 嵌套注释
    /* outer /* inner */ */
    
    // 正常代码
    int x = 42;
    )";
    runTest(commentErrorTest, "注释错误测试", lexer);

    // 6. 混合错误测试
    const char* mixedErrorTest = R"(
    // 多个错误在同一行
    int @var = 123.456.789 "unterminated
    
    // 错误后的恢复
    int valid_var = 42;  // 这应该能正确解析
    
    /* 未终止的注释和字符串混合
    "混合错误
    
    // 正常代码确认恢复
    float pi = 3.14;
    )";
    runTest(mixedErrorTest, "混合错误测试", lexer);

    return 0;
}
