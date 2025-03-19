#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

using namespace rp::frontend;

void printToken(const Token& token) {
    std::cout << std::left << std::setw(20) << token.getText() << " | Kind: " << std::setw(5)
              << static_cast<int>(token.getKind()) << " | Line: " << std::setw(4) << token.getLine()
              << " | Column: " << std::setw(4) << token.getColumn() << std::endl;
}

int main() {
    DiagnosticEngine diagEngine;
    Lexer lexer(&diagEngine);

    // 测试各种标识符和关键字
    const char* testCode = R"(
    // 1. 基本标识符测试
    variable
    myVariable
    my_variable
    MyClass
    
    // 2. 特殊标识符测试
    _identifier      // 下划线开头
    identifier_      // 下划线结尾
    _               // 单个下划线
    __identifier__  // 双下划线
    identifier123   // 包含数字
    _123           // 下划线加数字
    
    // 3. C++关键字测试
    alignas
    alignof
    asm
    auto
    bool
    break
    case
    catch
    char
    char16_t
    char32_t
    class
    const
    constexpr
    const_cast
    continue
    decltype
    default
    delete
    do
    double
    dynamic_cast
    else
    enum
    explicit
    export
    extern
    false
    float
    for
    friend
    goto
    if
    inline
    int
    long
    mutable
    namespace
    new
    noexcept
    nullptr
    operator
    private
    protected
    public
    register
    reinterpret_cast
    return
    short
    signed
    sizeof
    static
    static_assert
    static_cast
    struct
    switch
    template
    this
    thread_local
    throw
    true
    try
    typedef
    typeid
    typename
    union
    unsigned
    using
    virtual
    void
    volatile
    wchar_t
    while
    
    // 4. 标识符上下文测试
    int myVariable = 42;
    class MyClass {
        private:
            int _privateVar;
        public:
            void myMethod();
    };
    
    // 5. 错误测试用例
    123identifier   // 数字开头
    my-variable    // 非法字符
    my variable    // 空格
    )";

    lexer.setSource(testCode, strlen(testCode), "identifier_test.cpp");

    std::cout << "=== 标识符和关键字测试 ===" << std::endl;
    std::cout << std::left << std::setw(20) << "Token"
              << " | " << std::setw(10) << "Kind"
              << " | " << std::setw(8) << "Line"
              << " | " << std::setw(8) << "Column" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    // 读取并打印所有token
    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) {
            break;
        }
        // 只打印标识符和关键字
        if (token.getKind() == TokenKind::Identifier || token.isKeyword()) {
            printToken(token);
        }
    }

    // 测试Unicode标识符
    const char* unicodeTestCode = R"(
    // Unicode标识符测试
    变量
    クラス
    переменная
    μεταβλητή
    变量_123
    _変数
    )";

    std::cout << "\n=== Unicode标识符测试 ===" << std::endl;
    lexer.setSource(unicodeTestCode, strlen(unicodeTestCode), "unicode_identifier_test.cpp");

    while (true) {
        Token token = lexer.nextToken();
        if (token.getKind() == TokenKind::EndOfFile) {
            break;
        }
        if (token.getKind() == TokenKind::Identifier) {
            printToken(token);
        }
    }

    return 0;
}
