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

## 数字字面量处理

### NumberLiteralLexer 类
```cpp
class NumberLiteralLexer {
public:
    explicit NumberLiteralLexer(std::shared_ptr<DiagnosticEngine> diagnostics);
    void setSource(const char *src, size_t length, const std::string &file);
    Token scan();
    size_t currentPos;
    size_t currentLine;
    size_t currentColumn;
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
};
```
