# StringLiteralLexer

## 枚举

### StringPrefix
字符串字面量前缀类型
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
字符串错误类型
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
字符串引号类型
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

### StringScanResult
字符串扫描结果
```cpp
struct StringScanResult {
    Token token;                        // 处理后的Token
    bool success;                       // 是否成功
    std::string error;                  // 错误信息
    size_t errorPosition;               // 错误位置
    bool hasWarnings;                   // 是否有警告
    std::vector<std::string> warnings;  // 警告信息列表
};
```

### StringValidationOptions
字符串验证选项
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

### EscapeSequenceResult
转义序列处理结果
```cpp
struct EscapeSequenceResult {
    std::string value;     // 处理后的字符串值
    size_t consumed;       // 消耗的字符数
    bool success;          // 是否成功
    std::string error;     // 错误信息
    size_t errorPosition;  // 错误位置
};
```

## 类

### StringLiteralLexer
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

### StringLiteralUtils
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
    static bool isWhitespace(char c);
    static bool isValidStringChar(unsigned char c);
    static bool hasValidQuotes(const std::string& str);
    static bool isEscaped(const std::string& str, size_t pos);
    static bool isUnescapedQuote(const std::string& str, size_t pos);
    static std::string getErrorMessage(StringError error, size_t pos = 0, const std::string& context = "");
    static bool isValidRawStringDelimiter(const std::string& delimiter);
};
```

### EscapeSequenceProcessor
```cpp
class EscapeSequenceProcessor {
public:
    static std::string processEscapeSequence(const std::string& source, size_t& currentPos, std::string& error);
    static bool isValidEscapeSequence(char c);
    static std::optional<size_t> getExpectedLength(char escapeChar);
};
```

### NormalStringProcessor
```cpp
class NormalStringProcessor {
public:
    static StringProcessResult processNormalStringLiteral(const std::string& source, size_t& currentPos, const SourceLocation& startLoc);
    static QuoteType getQuoteType(const std::string& source, size_t pos, size_t& quoteLength);
};
```

### RawStringProcessor
```cpp
class RawStringProcessor {
public:
    static RawStringResult processRawStringLiteral(const std::string& source, size_t& currentPos, const SourceLocation& startLoc);
};
```
