#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

using namespace rp::frontend;

void printToken(const Token& token) {
    std::cout << std::left << std::setw(20) << token.getText() << " | Kind: " << std::setw(5)
              << static_cast<int>(token.getKind()) << " | Line: " << std::setw(4) << token.getLine()
              << " | Column: " << std::setw(4) << token.getColumn() << std::endl;
}

void runOperatorTest(Lexer& lexer) {
    const char* operatorTest = R"(
    // 算术运算符
    + - * / % ++ -- += -= *= /= %=
    
    // 比较运算符
    == != < > <= >= <=>
    
    // 逻辑运算符
    && || ! &= |= ^= and or not
    
    // 位运算符
    & | ^ ~ << >> <<= >>=
    
    // 其他运算符
    = -> . .* :: ?: sizeof... co_await
    
    // 标点符号
    { } [ ] ( ) ; : , ... #
    
    // 复合使用
    a->b.c::d++;
    (*ptr)->method();
    arr[i++] *= 2;
    )";

    std::cout << "\n=== 运算符测试 ===" << std::endl;
    lexer.setSource(operatorTest, strlen(operatorTest), "operator_test.cpp");

    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) break;
        printToken(token);
    }
}

void runNestedStructureTest(Lexer& lexer) {
    const char* nestedTest = R"(
    template<typename T>
    class OuterClass {
        template<typename U>
        struct InnerStruct {
            T data1;
            U data2;
            
            template<typename V>
            V method() {
                return static_cast<V>(data1 + data2);
            }
        };
        
        void complexFunction() {
            auto lambda = [this]<typename X>(X&& x) -> decltype(auto) {
                if constexpr (std::is_same_v<X, int>) {
                    return std::forward<X>(x);
                } else {
                    return InnerStruct<X>{};
                }
            };
        }
    };
    )";

    std::cout << "\n=== 嵌套结构测试 ===" << std::endl;
    lexer.setSource(nestedTest, strlen(nestedTest), "nested_test.cpp");

    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) break;
        printToken(token);
    }
}

void runCommentTest(Lexer& lexer) {
    const char* commentTest = R"(
    // 单行注释测试
    int x = 42; // 行尾注释
    
    /* 多行注释测试
     * 第二行
     * 第三行
     */
    
    int y = 43; /* 行内注释 */ int z = 44;
    
    /************
     * 花式注释 *
     ************/
    
    // 注释中的特殊字符: @#$%^&*
    /* 注释中的引号: "string" 'c' */
    
    // 注释中的代码
    // int test = 42;
    /* void func() {
        return;
    } */
    )";

    std::cout << "\n=== 注释测试 ===" << std::endl;
    lexer.setSource(commentTest, strlen(commentTest), "comment_test.cpp");

    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) break;
        printToken(token);
    }
}

void runPerformanceTest(Lexer& lexer) {
    // 生成大量代码
    std::string largeCode;
    for (int i = 0; i < 1000; ++i) {
        largeCode += R"(
        template<typename T>
        T performanceTest(T x, T y) {
            auto result = (x + y) * (x - y);
            if (result > 0) {
                return result << 2;
            } else {
                return result >> 2;
            }
        }
        )";
    }

    std::cout << "\n=== 性能测试 ===" << std::endl;
    std::cout << "处理代码行数: " << std::count(largeCode.begin(), largeCode.end(), '\n') << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    lexer.setSource(largeCode.c_str(), largeCode.length(), "performance_test.cpp");
    int tokenCount = 0;

    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) break;
        ++tokenCount;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "处理时间: " << duration.count() << "ms" << std::endl;
    std::cout << "Token数量: " << tokenCount << std::endl;
    std::cout << "每秒处理Token数: " << static_cast<double>(tokenCount) / (duration.count() / 1000.0) << std::endl;
}

int main() {
    DiagnosticEngine diagEngine;
    Lexer lexer(&diagEngine);

    // 运行各种测试
    runOperatorTest(lexer);
    runNestedStructureTest(lexer);
    runCommentTest(lexer);
    runPerformanceTest(lexer);

    return 0;
}
