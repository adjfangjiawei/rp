# StringLiteralLexer 模块文档

## 主模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/DiagnosticsHandler.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/EscapeSequenceProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/NormalStringProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/PrefixProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/RawStringProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/StringLiteralUtils.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/StringValidator.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/TokenCreator.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/UnicodeProcessor.h"
```

命名空间:
```cpp
rp::frontend
```

## 枚举类型

### StringPrefix
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

### StringError
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

### QuoteType
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

## 结构体

### StringProcessResult
```cpp
struct StringProcessResult {
    Token token;                        // 处理后的Token
    bool success;                       // 是否成功
    std::string error;                  // 错误信息
    size_t errorPosition;               // 错误位置
    bool hasWarnings;                   // 是否有警告
    std::vector<std::string> warnings;  // 警告信息列表
};
```

### RawStringResult
```cpp
struct RawStringResult {
    Token token;                        // 处理后的Token
    bool success;                       // 是否成功
    std::string error;                  // 错误信息
    size_t errorPosition;               // 错误位置
    bool hasWarnings;                   // 是否有警告
    std::vector<std::string> warnings;  // 警告信息列表
};
```

### EscapeSequenceResult
```cpp
struct EscapeSequenceResult {
    std::string value;     // 处理后的字符串值
    size_t consumed;       // 消耗的字符数
    bool success;          // 是否成功
    std::string error;     // 错误信息
    size_t errorPosition;  // 错误位置
};
```

### StringValidationOptions
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

## 类

### DiagnosticsHandler
```cpp
class DiagnosticsHandler {
    explicit DiagnosticsHandler(const std::shared_ptr<DiagnosticEngine>& diagnostics);
    void handleDiagnostics(const std::string& error,
                          const std::vector<std::string>& warnings,
                          const SourceLocation& loc);
    void reportError(const std::string& error, const SourceLocation& loc);
    void reportWarning(const std::string& warning, const SourceLocation& loc);
};
```

### EscapeSequenceProcessor
```cpp
class EscapeSequenceProcessor {
    static std::string processEscapeSequence(const std::string& source, size_t& currentPos, std::string& error);
    static bool isValidEscapeSequence(char c);
    static std::optional<size_t> getExpectedLength(char escapeChar);
};
```

### NormalStringProcessor
```cpp
class NormalStringProcessor {
    static StringProcessResult processNormalStringLiteral(const std::string& source,
                                                        size_t& currentPos,
                                                        const SourceLocation& startLoc);
    static QuoteType getQuoteType(const std::string& source, size_t pos, size_t& quoteLength);
};
```

### PrefixProcessor
```cpp
class PrefixProcessor {
    static std::tuple<StringPrefix, size_t> parsePrefix(const std::string& source,
                                                      size_t currentPos,
                                                      size_t sourceLength);
    static bool isValidPrefix(const std::string& prefix);
    static std::string getPrefixString(StringPrefix prefix);
};
```

### RawStringProcessor
```cpp
class RawStringProcessor {
    static RawStringResult processRawStringLiteral(const std::string& source,
                                                 size_t& currentPos,
                                                 const SourceLocation& startLoc);
};
```

### StringLiteralUtils
```cpp
class StringLiteralUtils {
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
    static bool isWhitespace(char c);
    static bool isValidStringChar(unsigned char c);
    static bool hasValidQuotes(const std::string& str);
    static bool isEscaped(const std::string& str, size_t pos);
    static bool isUnescapedQuote(const std::string& str, size_t pos);
    static std::string getErrorMessage(StringError error, size_t pos = 0, const std::string& context = "");
    static bool isValidRawStringDelimiter(const std::string& delimiter);
};
```

### StringValidator
```cpp
class StringValidator {
    explicit StringValidator(const StringValidationOptions& options);
    bool validate(const std::string& content, std::string& error, std::vector<std::string>& warnings);
};
```

### TokenCreator
```cpp
class TokenCreator {
    static Token createStringToken(const std::string& content,
                                 StringPrefix prefix,
                                 size_t line,
                                 size_t column,
                                 const std::string& filename);
};
```

### UnicodeProcessor
```cpp
class UnicodeProcessor {
    static std::string processUnicodeEscape(const std::string& source,
                                          size_t& currentPos,
                                          bool isLongForm,
                                          std::string& error);
    static std::string codePointToUTF8(uint32_t codepoint);
    static bool isValidCodePoint(uint32_t codepoint);
    static std::string processHexEscape(const std::string& source, size_t& currentPos, std::string& error);
};
```
