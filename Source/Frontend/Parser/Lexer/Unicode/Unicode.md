# Unicode 模块文档

## 主模块
使用#include "Frontend/Parser/Lexer/Unicode/Unicode.h"就能使用模块所有内容

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Unicode/Unicode.h"
```

命名空间:
```cpp
rp::frontend::unicode
```

类型别名:
```cpp
using CodePoint = uint32_t
using Char32 = char32_t
using StringView = std::string_view
```

常量:
```cpp
constexpr uint32_t SURROGATE_MIN = 0xD800
constexpr uint32_t SURROGATE_MAX = 0xDFFF
```

结构体:
```cpp
struct UnicodeError {
    std::string message;
    size_t position;
    std::string context;
};

struct ValidationResult {
    bool valid;
    std::vector<UnicodeError> errors;
};
```

便利函数:
```cpp
ValidationResult validateString(const std::string& str)
std::vector<CodePoint> toCodePoints(const std::string& str)
std::string fromCodePoints(const std::vector<CodePoint>& codepoints)
std::string formatCodePoint(CodePoint codepoint)
bool isCategory(const std::string& str, UnicodeCategories::Category category)
size_t displayWidth(const std::string& str)
std::string normalizeLineEndings(const std::string& str, const std::string& style = "LF")
```

## Categories 模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Unicode/Categories/UnicodeCategories.h"
```

命名空间:
```cpp
rp::frontend::unicode
```

类:
```cpp
class UnicodeCategories
```

枚举:
```cpp
enum class Category {
    Letter_Uppercase,      // 大写字母
    Letter_Lowercase,      // 小写字母
    Letter_Titlecase,      // 标题大小写字母
    Letter_Modifier,       // 修饰字母
    Letter_Other,         // 其他字母
    Number_Decimal,       // 十进制数字
    Number_Letter,        // 字母数字
    Number_Other,         // 其他数字
    Punctuation_Connector, // 连接标点
    Punctuation_Dash,     // 破折号标点
    Punctuation_Open,     // 开括号标点
    Punctuation_Close,    // 闭括号标点
    Punctuation_Quote,    // 引号标点
    Punctuation_Other,    // 其他标点
    Symbol_Math,          // 数学符号
    Symbol_Currency,      // 货币符号
    Symbol_Modifier,      // 修饰符号
    Symbol_Other,         // 其他符号
    Separator_Space,      // 空格分隔符
    Separator_Line,       // 行分隔符
    Separator_Paragraph,  // 段落分隔符
    Mark_NonSpacing,      // 非空格标记
    Mark_SpacingCombining, // 空格组合标记
    Mark_Enclosing,      // 包围标记
    Other_Control,        // 控制字符
    Other_Format,         // 格式字符
    Other_Surrogate,      // 代理项字符
    Other_PrivateUse,    // 私用字符
    Other_NotAssigned    // 未分配字符
}
```

公共方法:
```cpp
static bool isIdentifierStart(uint32_t codepoint)
static bool isIdentifierContinue(uint32_t codepoint)
static bool isWhitespace(uint32_t codepoint)
static bool isDigit(uint32_t codepoint)
static bool isHexDigit(uint32_t codepoint)
static bool isAlpha(uint32_t codepoint)
static bool isAlnum(uint32_t codepoint)
static Category getCategory(uint32_t codepoint)
static bool isInCategory(uint32_t codepoint, Category category)
```

## Core 模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Unicode/Core/UnicodeCore.h"
```

命名空间:
```cpp
rp::frontend::unicode
```

类:
```cpp
class UnicodeCore
```

公共方法:
```cpp
static bool isValidCodepoint(uint32_t codepoint)
static bool isValidUtf8FirstByte(unsigned char byte)
static bool isUtf8ContinuationByte(unsigned char byte)
static bool isOverlongEncoding(uint32_t codepoint, size_t length)
static Utf8SequenceInfo getUtf8SequenceInfo(const std::string &str, size_t start)
static size_t getUtf8ByteCount(uint32_t codepoint)
static size_t getUtf8SequenceLength(unsigned char firstByte)
```

## Encoding 模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"
```

命名空间:
```cpp
rp::frontend::unicode
```

类:
```cpp
class UnicodeEncoding
```

公共方法:
```cpp
static std::string encodeUtf8(uint32_t codepoint)
static bool decodeUtf8(const std::string &utf8, std::vector<uint32_t> &codepoints)
static uint32_t utf8ToCodePoint(std::string_view sv, size_t &bytesRead)
static std::string codePointToUtf8(uint32_t codePoint)
static std::tuple<uint32_t, size_t> getMultiByteChar(const std::string &str, size_t start)
static bool isMultiByteChar(const std::string &str, size_t start)
static size_t getMultiByteCharLength(unsigned char firstByte)
```

## Escape 模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Unicode/Escape/UnicodeEscape.h"
```

命名空间:
```cpp
rp::frontend::unicode
```

类:
```cpp
class UnicodeEscape
```

公共方法:
```cpp
static EscapeResult parseEscapeSequence(const std::string& input, size_t start)
static std::string generateEscapeSequence(uint32_t codepoint)
static bool validateEscapeSequence(const std::string& input, std::string& error)
static EscapeResult processLineContinuation(const std::string& input)
static bool isHexDigit(char c)
static uint32_t hexDigitToValue(char c)
static bool isOctalDigit(char c)
static uint32_t octalDigitToValue(char c)
```

## Processing 模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Unicode/Processing/UnicodeProcessing.h"
```

命名空间:
```cpp
rp::frontend::unicode
```

类:
```cpp
class UnicodeProcessing
```

公共方法:
```cpp
static CharacterResult processCharacter(const std::string &str, size_t start)
static CharacterResult processUtf8Character(const std::string &str, size_t start)
static StringResult processUtf8String(const std::string &str, size_t start, size_t length)
static std::string normalize(const std::string &str, bool compose = true)
static std::string toUpper(const std::string &str)
static std::string toLower(const std::string &str)
static std::string toTitleCase(const std::string &str)
static size_t utf8Length(const std::string &str)
static std::string_view getNextUtf8Char(const std::string &str, size_t &pos)
static bool isValidUtf8(const std::string &str)
static bool validateUtf8Sequence(const std::string &str, size_t start, size_t &bytesConsumed)
static bool validateUtf8String(const std::string &str, std::string &error)
static int getCharWidth(uint32_t codepoint)
```

## Scanner 模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Unicode/Scanner/UTF8Scanner.h"
```

命名空间:
```cpp
rp::frontend::unicode
```

类:
```cpp
class UTF8Scanner
```

公共方法:
```cpp
UTF8Scanner(const std::string& input)
void reset()
bool hasMore() const
size_t position() const
void setPosition(size_t pos)
std::string scanUTF8Sequence()
uint32_t decodeUTF8Sequence(char first)
bool isValidUTF8Continuation(char c) const
ScanError getLastError() const
void skipInvalidUTF8()
bool tryPeekCodepoint(uint32_t& codepoint) const
size_t lookAhead(size_t n) const
std::string peekString(size_t length) const
bool skipUntil(uint32_t targetCodepoint)
std::string collectUntil(uint32_t targetCodepoint)
```
