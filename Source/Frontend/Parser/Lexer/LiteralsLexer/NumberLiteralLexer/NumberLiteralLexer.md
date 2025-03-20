# NumberLiteralLexer 模块文档

## 主模块

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"
```

命名空间:
```cpp
rp::frontend
```

## 基础结构

### NumberKind 枚举
```cpp
enum class NumberKind {
    Integer,           // 整数
    FloatingPoint,     // 浮点数
    Binary,           // 二进制
    Octal,            // 八进制
    Hexadecimal       // 十六进制
};
```

### NumberValue 结构体
```cpp
struct NumberValue {
    double value;          // 数值
    NumberKind kind;       // 数字类型
    bool isUnsigned;       // 是否无符号
    bool isLong;          // 是否为long
    bool isLongLong;      // 是否为long long
    bool isFloat;         // 是否为float
    bool isDouble;        // 是否为double
};
```

## 基础类

### NumberLiteralBase
```cpp
class NumberLiteralBase
```

公共方法:
```cpp
static bool isDigit(uint32_t codepoint)                    // 检查是否为数字
static bool isHexDigit(uint32_t codepoint)                 // 检查是否为十六进制数字
static bool isBinaryDigit(uint32_t codepoint)              // 检查是否为二进制数字
static bool isOctalDigit(uint32_t codepoint)               // 检查是否为八进制数字
static bool isNumberSeparator(uint32_t codepoint)          // 检查是否为数字分隔符
static int digitValue(uint32_t codepoint)                  // 获取数字值
static int hexDigitValue(uint32_t codepoint)               // 获取十六进制数字值
static bool validateNumberLiteral(const std::string &str,   // 验证数字字面量
                                std::string &error)
```

## 二进制字面量处理

### BinaryLiteralLexer
```cpp
class BinaryLiteralLexer : public NumberLiteralBase
```

公共方法:
```cpp
static bool processBinaryLiteral(const std::string &input,  // 处理二进制字面量
                                size_t &pos,
                                NumberValue &value,
                                std::string &error)
```

## 浮点数字面量处理

### FloatLiteralLexer
```cpp
class FloatLiteralLexer : public NumberLiteralBase
```

公共方法:
```cpp
static bool processFloatingLiteral(const std::string &input,  // 处理浮点数字面量
                                  size_t &pos,
                                  NumberValue &value,
                                  std::string &error)
```

## 十六进制字面量处理

### HexLiteralLexer
```cpp
class HexLiteralLexer : public NumberLiteralBase
```

公共方法:
```cpp
static bool processHexLiteral(const std::string &input,     // 处理十六进制字面量
                             size_t &pos,
                             NumberValue &value,
                             std::string &error)
```

## 整数字面量处理

### IntegerLiteralLexer
```cpp
class IntegerLiteralLexer : public NumberLiteralBase
```

公共方法:
```cpp
static bool processIntegerLiteral(const std::string &input,  // 处理整数字面量
                                 size_t &pos,
                                 NumberValue &value,
                                 std::string &error)
```

## 八进制字面量处理

### OctalLiteralLexer
```cpp
class OctalLiteralLexer : public NumberLiteralBase
```

公共方法:
```cpp
static bool processOctalLiteral(const std::string &input,    // 处理八进制字面量
                               size_t &pos,
                               NumberValue &value,
                               std::string &error)
```

## 后缀处理

### SuffixProcessor
```cpp
class SuffixProcessor
```

公共方法:
```cpp
static bool processSuffix(const std::string &input,         // 处理数字字面量后缀
                         size_t &pos,
                         NumberValue &value,
                         std::string &error)
```

枚举:
```cpp
enum class SuffixState {
    Start,            // 开始状态
    AfterU,           // U后缀之后
    AfterL,           // L后缀之后
    AfterLL,          // LL后缀之后
    AfterF,           // F后缀之后
    AfterD            // D后缀之后
};
```
