# MainLexer 模块文档

## 主模块
使用#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"就能使用模块所有内容

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
```

命名空间:
```cpp
rp::frontend
```

## 基础类

### BaseScanner 类
```cpp
class BaseScanner {
    explicit BaseScanner(std::shared_ptr<DiagnosticEngine> diagEngine);
    virtual ~BaseScanner() = default;

    // 源代码管理
    virtual void setSource(const char* src, size_t length, const std::string& filename);

    // 位置管理
    size_t getCurrentPos() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    virtual void setPosition(size_t pos, size_t line, size_t column);

    // 源码范围检查
    bool isEndOfFile() const;
    bool hasMoreChars() const;
    size_t getRemainingLength() const;

protected:
    // Token创建辅助函数
    Token createToken(TokenKind kind, const std::string& text = "", size_t startPos = 0);
    Token createToken(TokenKind kind, size_t startPos, size_t length);

    // 安全的字符访问
    char getCurrentChar() const;
    char peekChar(size_t offset = 1) const;

    // 字符处理辅助函数
    bool isAtLineEnd() const;
    void skipLineEnd();
    void skipWhitespace();
    bool matchString(const char* str, size_t length) const;
    bool matchChar(char c) const;

    // 位置计算辅助函数
    void calculateLineAndColumn(size_t pos, size_t& line, size_t& column) const;
    std::pair<size_t, size_t> getLineAndColumn(size_t pos) const;

    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
};
```

### IdentifierScanner 类
```cpp
class IdentifierScanner : public UTF8Scanner {
    // 标识符扫描
    Token scanIdentifier();

    // 字符判断函数
    bool isIdentifierStart(char c) const;
    bool isIdentifierContinue(char c) const;
    bool isUnicodeIdentifierStart(uint32_t codepoint) const;
    bool isUnicodeIdentifierContinue(uint32_t codepoint) const;
};
```

### KeywordManager 类
```cpp
class KeywordManager {
    // 关键字检查
    static bool isKeyword(std::string_view text, TokenKind& kind);
    static bool isContextualKeyword(std::string_view text, TokenKind& kind);

    // 关键字验证
    static bool isValidKeyword(std::string_view text);
    static bool requiresContext(TokenKind kind);

    // 关键字信息
    static size_t getMinKeywordLength();
    static size_t getMaxKeywordLength();
};
```

### Lexer 类
```cpp
class Lexer {
    // 构造函数
    Lexer();
    explicit Lexer(std::shared_ptr<DiagnosticEngine> diagEngine);
    ~Lexer() = default;

    // 源代码管理
    void setSource(const char* src, size_t length, const std::string& filename);

    // Token操作
    Token nextToken();
    Token peekToken();
    Token peekToken(size_t n);
    void ungetToken(const Token& token);

    // 位置信息
    std::pair<size_t, size_t> getCurrentPosition() const;
    std::string getErrorContext(size_t line, size_t column, size_t context_lines = 2) const;

    // 错误处理
    void reportError(const std::string& message, size_t line, size_t column);
    void reportWarning(const std::string& message, size_t line, size_t column);
    std::shared_ptr<DiagnosticEngine> getDiagnostics() const;

protected:
    // Token处理
    Token createToken(TokenKind kind, const std::string& text = "", bool consumeToken = true);
    void saveTokenStart();
    void restoreToTokenStart();
    void updatePositionFromScanner();
};
```

### OperatorScanner 类
```cpp
class OperatorScanner : public BaseScanner {
    explicit OperatorScanner(std::shared_ptr<DiagnosticEngine> diagEngine);

    // 运算符和标点符号扫描
    Token scanOperatorOrPunctuation();
};
```

### Scanner 类
```cpp
class Scanner {
    explicit Scanner(std::shared_ptr<DiagnosticEngine> diagEngine);

    // 源代码管理
    void setSource(const char* src, size_t length, const std::string& filename);

    // 主要扫描函数
    Token scanIdentifier();
    Token scanOperatorOrPunctuation();
    Token scanNumber();
    Token scanString();
    Token scanCharacter();
    Token scanComment();
    Token scanPreprocessor();

    // 位置管理
    size_t getCurrentPos() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    void setPosition(size_t pos, size_t line, size_t column);

    // 字符判断函数
    bool isIdentifierStart(char c) const;
    bool isIdentifierContinue(char c) const;
    bool isDigit(char c) const;
    bool isHexDigit(char c) const;
    bool isOctalDigit(char c) const;
    bool isWhitespace(char c) const;

    // 辅助函数
    void skipWhitespace();
    void skipUntilNewline();
    bool lookAhead(const std::string& str) const;
    std::optional<char> peekChar(size_t offset = 1) const;

    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    void skipInvalidToken();
};
```

### UTF8Scanner 类
```cpp
class UTF8Scanner : public BaseScanner {
    explicit UTF8Scanner(std::shared_ptr<DiagnosticEngine> diagEngine);
    ~UTF8Scanner() override = default;

    // 重写基类方法
    void setSource(const char* src, size_t length, const std::string& filename) override;

protected:
    // UTF-8序列处理
    std::string scanUTF8Sequence();
    bool isValidUTF8Continuation(char c) const;
    bool isValidUTF8FirstByte(char c) const;
    size_t getUTF8SequenceLength(char firstByte) const;

    // UTF-8字符检查
    bool isUTF8Char() const;
    std::optional<uint32_t> tryPeekCodepoint() const;
    size_t lookAheadUTF8(size_t n) const;

    // UTF-8错误处理
    void reportInvalidUTF8(const std::string& detail = "");
    void skipInvalidUTF8();

    // UTF-8序列解码
    uint32_t decodeUTF8Sequence(char first);
    std::pair<uint32_t, size_t> getNextCodepoint();

    // UTF-8字符串处理
    std::string collectUTF8Until(uint32_t targetCodepoint);
    bool skipUTF8Until(uint32_t targetCodepoint);
};
```
