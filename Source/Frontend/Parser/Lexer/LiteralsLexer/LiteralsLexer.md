# LiteralsLexer 模块文档

## 主模块
使用以下头文件就能使用模块所有内容：
```cpp
#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
```

命名空间:
```cpp
rp::frontend
```

## 字符串字面量处理

### StringScanResult 结构体
```cpp
struct StringScanResult {
    Token token;                        // 处理后的Token
    bool success;                       // 是否成功
    std::string error;                  // 错误信息
    size_t errorPosition;               // 错误位置
    bool hasWarnings;                   // 是否有警告
    std::vector<std::string> warnings;  // 警告信息列表
    size_t consumed;                    // 消耗的字符数
};
```

### StringValidationOptions 结构体
```cpp
struct StringValidationOptions {
    bool allowControlChars;    // 是否允许控制字符
    bool strictUTF8;           // 是否严格检查UTF-8
    bool allowInvalidUnicode;  // 是否允许无效的Unicode
    bool allowMultiline;       // 是否允许多行字符串
    size_t maxLength;          // 最大字符串长度
    size_t maxLines;           // 最大行数
    bool allowChineseQuotes;   // 是否允许中文引号
    bool allowSmartQuotes;     // 是否允许智能引号
};
```

### StringLiteralLexer 类
```cpp
class StringLiteralLexer {
public:
    explicit StringLiteralLexer(const std::shared_ptr<DiagnosticEngine>& diagnostics);
    void setSource(const std::string& src, size_t length, const std::string& filename);
    StringScanResult scan();
    void setValidationOptions(const StringValidationOptions& options);
    size_t getCurrentPos() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    void setPosition(size_t pos, size_t line, size_t column);
};
```

### StringPrefix 枚举
```cpp
enum class StringPrefix {
    None,  // 无前缀
    L,     // L"..."
    u,     // u"..."
    U,     // U"..."
    u8,    // u8"..."
    R,     // R"..."
    LR,    // LR"..."
    uR,    // uR"..."
    UR,    // UR"..."
    u8R    // u8R"..."
};
```

### StringError 枚举
```cpp
enum class StringError {
    None,
    InvalidUTF8Sequence,
    UnterminatedString,
    InvalidEscapeSequence,
    InvalidDelimiter,
    InvalidPrefix
};
```

### QuoteType 枚举
```cpp
enum class QuoteType {
    None,
    DoubleQuote,       // "
    SingleQuote,       // '
    ChineseQuote,      // 「」
    ChineseBookQuote,  // 『』
    SmartQuote,        // ''
    SmartDoubleQuote   // ""
};
```

### EscapeSequenceResult 结构体
```cpp
struct EscapeSequenceResult {
    std::string value;     // 处理后的字符串值
    size_t consumed;       // 消耗的字符数
    bool success;          // 是否成功
    std::string error;     // 错误信息
    size_t errorPosition;  // 错误位置
};
```

### UTF8ProcessResult 结构体
```cpp
struct UTF8ProcessResult {
    bool success;
    size_t consumed;
    std::string content;
    std::vector<std::string> warnings;
    std::string error;
};
```

### DiagnosticsHandler 类
```cpp
class DiagnosticsHandler {
public:
    explicit DiagnosticsHandler(const std::shared_ptr<DiagnosticEngine>& diagnostics);
    void handleDiagnostics(const std::string& error,
                          const std::vector<std::string>& warnings,
                          const SourceLocation& loc);
    void reportError(const std::string& error, const SourceLocation& loc);
    void reportWarning(const std::string& warning, const SourceLocation& loc);
};
```

### EscapeSequenceProcessor 类
```cpp
class EscapeSequenceProcessor {
public:
    static std::string processEscapeSequence(const std::string& source, size_t& currentPos, std::string& error);
    static bool isValidEscapeSequence(char c);
    static std::optional<size_t> getExpectedLength(char escapeChar);
};
```

### PrefixProcessor 类
```cpp
class PrefixProcessor {
public:
    static std::tuple<StringPrefix, size_t> parsePrefix(const std::string& source,
                                                       size_t currentPos,
                                                       size_t sourceLength);
    static bool isValidPrefix(const std::string& prefix);
    static std::string getPrefixString(StringPrefix prefix);
};
```

### StringLiteralUtils 类
```cpp
class StringLiteralUtils {
public:
    static bool isOctalDigit(char c);
    static bool isHexDigit(char c);
    static int hexDigitToInt(char c);
    static std::string unicodeToUTF8(unsigned int codepoint);
    static bool isValidUTF8StartByte(unsigned char c);
    static size_t getUTF8ByteCount(unsigned char c);
    static bool isValidUTF8ContinuationByte(unsigned char c);
    static std::tuple<bool, size_t> validateUTF8Sequence(const std::string& str, size_t pos);
    static bool validateCompleteUTF8String(const std::string& str, std::string& errorMsg);
    static std::tuple<uint32_t, size_t> getUTF8Char(const std::string& str, size_t pos);
    static std::tuple<StringPrefix, size_t> parseStringPrefix(const std::string& input);
    static bool isValidStringPrefix(const std::string& prefix);
    static bool isRawStringPrefix(StringPrefix prefix);
    static std::string getPrefixString(StringPrefix prefix);
    static TokenKind getPrefixTokenKind(StringPrefix prefix);
};
```

### StringValidator 类
```cpp
class StringValidator {
public:
    explicit StringValidator(const StringValidationOptions& options);
    bool validate(const std::string& content, std::string& error, std::vector<std::string>& warnings);
};
```

### TokenCreator 类
```cpp
class TokenCreator {
public:
    static Token createStringToken(const std::string& content,
                                 StringPrefix prefix,
                                 size_t line,
                                 size_t column,
                                 const std::string& filename);
};
```

### UnicodeProcessor 类
```cpp
class UnicodeProcessor {
public:
    static std::string processUnicodeEscape(const std::string& source,
                                          size_t& currentPos,
                                          bool isLongForm,
                                          std::string& error);
    static std::string codePointToUTF8(uint32_t codepoint);
    static bool isValidCodePoint(uint32_t codepoint);
    static std::string processHexEscape(const std::string& source, size_t& currentPos, std::string& error);
};
```

## 数字字面量处理

### NumberLiteralLexer 类
```cpp
class NumberLiteralLexer {
public:
    explicit NumberLiteralLexer(std::shared_ptr<DiagnosticEngine> diagnostics);
    void setSource(const char *src, size_t length, const std::string &file);
    Token scan();
    size_t getCurrentPos() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    void setPosition(size_t pos, size_t line, size_t column);
};
```

## 字符字面量处理

### CharacterLiteralLexer 类
```cpp
class CharacterLiteralLexer : public Lexer {
public:
    explicit CharacterLiteralLexer(std::shared_ptr<DiagnosticEngine> diagEngine = nullptr);
    Token scan();
    static bool processCharacterLiteral(const std::string &input,
                                      size_t &pos,
                                      long long &value,
                                      std::string &error);
    static bool validateCharacterLiteral(const std::string &str, std::string &error);
    size_t getCurrentPos() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    void setPosition(size_t pos, size_t line, size_t column);
};
```
