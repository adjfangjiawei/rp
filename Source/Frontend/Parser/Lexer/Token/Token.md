# Token 模块文档

## 主模块
使用#include "Frontend/Parser/Lexer/Token/Token.h"就能使用模块所有内容

包含头文件:
```cpp
#include "Frontend/Parser/Lexer/Token/Token.h"
```

命名空间:
```cpp
rp::frontend
```

## 基础结构

### Token 结构体
```cpp
struct Token {
    TokenKind kind;                    // Token类型
    std::string_view text;            // Token文本内容
    unsigned line;                    // 行号
    unsigned column;                  // 列号
    std::string filename;            // 文件名
    
    // 字面量值存储
    union {
        long long intValue;          // 整数值
        double floatValue;           // 浮点数值
        bool boolValue;              // 布尔值
    };
};
```

### 字符串信息结构体
```cpp
struct StringInfo {
    bool isRawString;               // 是否为原始字符串
    std::string delimiter;          // 原始字符串分隔符
    bool isWide;                    // 是否为宽字符串
    bool isUTF8;                    // 是否为UTF-8字符串
    bool isUTF16;                   // 是否为UTF-16字符串
    bool isUTF32;                   // 是否为UTF-32字符串
};
```

### 错误信息结构体
```cpp
struct ErrorInfo {
    bool hasError;                  // 是否存在错误
    std::string message;           // 错误信息
    unsigned errorLine;            // 错误行号
    unsigned errorColumn;          // 错误列号
};
```

## 枚举类型

### TokenCategory
```cpp
enum class TokenCategory {
    Special,                        // 特殊token（EOF, Invalid等）
    Literal,                        // 字面量
    Identifier,                     // 标识符
    Keyword,                        // 关键字
    Operator,                       // 运算符
    Delimiter,                      // 分隔符
    Attribute                       // 属性说明符
};
```

### TokenKind
```cpp
enum class TokenKind {
    // 基础Token
    EndOfFile,                    // end of file
    Invalid,                      // invalid token
    Identifier,                   // identifier
    NumberLiteral,               // number literal
    CharLiteral,                 // character literal
    LambdaIntro,                 // lambda introducer
    LambdaArrow,                 // lambda arrow

    // 字符串字面量
    StringLiteral,               // string literal
    RawStringLiteral,            // raw string literal
    WideStringLiteral,           // wide string literal
    UTF8StringLiteral,           // UTF-8 string literal
    UTF16StringLiteral,          // UTF-16 string literal
    UTF32StringLiteral,          // UTF-32 string literal
    StringPrefix_L,              // L
    StringPrefix_u8,             // u8
    StringPrefix_u,              // u
    StringPrefix_U,              // U
    StringPrefix_R,              // R
    StringLiteral_Unterminated,  // unterminated string literal
    StringLiteral_InvalidEscape, // invalid escape sequence in string literal
    StringLiteral_InvalidUTF8,   // invalid UTF-8 sequence in string literal
    StringLiteral_InvalidDelimiter, // invalid delimiter in raw string literal

    // 关键字
    Keyword_Class,               // class
    Keyword_Struct,              // struct
    Keyword_Enum,               // enum
    Keyword_Union,              // union
    Keyword_Template,           // template
    Keyword_Typename,           // typename
    Keyword_Const,              // const
    Keyword_Static,             // static
    Keyword_Virtual,            // virtual
    Keyword_Override,           // override
    Keyword_Auto,               // auto
    Keyword_Register,           // register
    Keyword_Extern,             // extern
    Keyword_Mutable,            // mutable
    Keyword_Final,              // final
    Keyword_Public,             // public
    Keyword_Private,            // private
    Keyword_Protected,          // protected
    Keyword_Volatile,           // volatile
    Keyword_Inline,             // inline
    Keyword_Explicit,           // explicit
    Keyword_If,                 // if
    Keyword_Else,               // else
    Keyword_While,              // while
    Keyword_For,                // for
    Keyword_Do,                 // do
    Keyword_Break,              // break
    Keyword_Continue,           // continue
    Keyword_Return,             // return
    Keyword_Alignas,            // alignas
    Keyword_Alignof,            // alignof
    Keyword_Char16_t,           // char16_t
    Keyword_Char32_t,           // char32_t
    Keyword_Constexpr,          // constexpr
    Keyword_Decltype,           // decltype
    Keyword_Noexcept,           // noexcept
    Keyword_Nullptr,            // nullptr
    Keyword_Static_assert,      // static_assert
    Keyword_Thread_local,       // thread_local
    Keyword_Deprecated,         // deprecated
    Keyword_Fallthrough,        // fallthrough
    Keyword_Nodiscard,          // nodiscard
    Keyword_Maybe_unused,       // maybe_unused
    Keyword_Char8_t,            // char8_t
    Keyword_Concept,            // concept
    Keyword_Consteval,          // consteval
    Keyword_Constinit,          // constinit
    Keyword_co_await,           // co_await
    Keyword_co_return,          // co_return
    Keyword_co_yield,           // co_yield
    Keyword_Requires,           // requires
    Keyword_Module,             // module
    Keyword_Import,             // import
    Keyword_Export,             // export

    // 运算符
    Plus,                       // +
    Minus,                      // -
    Star,                       // *
    Slash,                      // /
    Percent,                    // %
    Caret,                      // ^
    Ampersand,                  // &
    Pipe,                       // |
    Tilde,                      // ~
    Exclaim,                    // !
    Equal,                      // =
    Less,                       // <
    Greater,                    // >
    PlusEqual,                  // +=
    MinusEqual,                 // -=
    StarEqual,                  // *=
    SlashEqual,                 // /=
    PercentEqual,               // %=
    CaretEqual,                 // ^=
    AmpEqual,                   // &=
    PipeEqual,                  // |=
    LessLess,                   // <<
    GreaterGreater,             // >>
    LessLessLess,               // <<<
    GreaterGreaterGreater,      // >>>
    EqualEqual,                 // ==
    ExclaimEqual,               // !=
    LessEqual,                  // <=
    GreaterEqual,               // >=
    AmpAmp,                     // &&
    PipePipe,                   // ||
    PlusPlus,                   // ++
    MinusMinus,                 // --
    Arrow,                      // ->
    ArrowStar,                  // ->*
    Spaceship,                  // <=>

    // 分隔符
    LParen,                     // (
    RParen,                     // )
    LBrace,                     // {
    RBrace,                     // }
    LSquare,                    // [
    RSquare,                    // ]
    Semicolon,                  // ;
    Comma,                      // ,
    Period,                     // .
    Ellipsis,                   // ...
    Question,                   // ?
    Colon,                      // :
    ColonColon,                // ::
    Hash,                      // #
    HashHash,                  // ##

    // 预处理指令
    Directive_Include,          // #include
    Directive_Define,           // #define
    Directive_Pragma,           // #pragma
    Directive_If,              // #if
    Directive_Ifdef,           // #ifdef
    Directive_Ifndef,          // #ifndef
    Directive_Else,            // #else
    Directive_Elif,            // #elif
    Directive_Endif,           // #endif
    Directive_Undef,           // #undef
    Directive_Line,            // #line
    Directive_Error,           // #error
    Directive_Warning,         // #warning

    // 内置类型关键字
    Keyword_Void,              // void
    Keyword_Bool,              // bool
    Keyword_Char,              // char
    Keyword_Short,             // short
    Keyword_Int,               // int
    Keyword_Long,              // long
    Keyword_Float,             // float
    Keyword_Double,            // double
    Keyword_Signed,            // signed
    Keyword_Unsigned           // unsigned
};
```

## 公共方法

### 构造函数
```cpp
Token()                                           // 默认构造函数
explicit Token(TokenKind k)                       // 基本构造函数
Token(TokenKind k, unsigned ln, unsigned col)     // 位置信息构造函数
Token(TokenKind k, unsigned ln, unsigned col,     // 完整构造函数
     const std::string& fname)
```

### 文本操作
```cpp
void setText(std::string_view sv)                // 设置文本内容（视图）
void setText(std::string&& str)                  // 设置文本内容（移动）
void setText(const std::string& str)             // 设置文本内容（拷贝）
std::string_view getText() const                 // 获取文本内容
```

### 错误处理
```cpp
void setError(const std::string& message,         // 设置错误信息
             unsigned errorLine = 0,
             unsigned errorColumn = 0)
bool hasError() const                            // 检查是否有错误
std::string getErrorMessage() const              // 获取错误信息
std::optional<std::pair<unsigned, unsigned>>     // 获取错误位置
getErrorLocation() const
```

### 字符串信息操作
```cpp
void setStringInfo(bool isRaw = false,           // 设置字符串信息
                  const std::string& delim = "")
void setStringEncoding(bool isWide = false,      // 设置字符串编码类型
                      bool isUTF8 = false,
                      bool isUTF16 = false,
                      bool isUTF32 = false)
bool isRawString() const                         // 检查是否为原始字符串
bool isWideString() const                        // 检查是否为宽字符串
bool isUTF8String() const                        // 检查是否为UTF-8字符串
bool isUTF16String() const                       // 检查是否为UTF-16字符串
bool isUTF32String() const                       // 检查是否为UTF-32字符串
std::string getDelimiter() const                 // 获取原始字符串分隔符
```

### 位置信息
```cpp
std::string getLocation() const                  // 获取完整位置信息
unsigned getLine() const                         // 获取行号
unsigned getColumn() const                       // 获取列号
const std::string& getFilename() const          // 获取文件名
```

### 值获取
```cpp
std::optional<long long> getIntValue() const     // 获取整数值
std::optional<double> getFloatValue() const      // 获取浮点数值
std::optional<bool> getBoolValue() const         // 获取布尔值
```

### Token类型判断
```cpp
bool isKeyword() const                          // 是否为关键字
bool isOperator() const                         // 是否为运算符
bool isDelimiter() const                        // 是否为分隔符
TokenKind getKind() const                       // 获取Token类型
```

### 比较操作
```cpp
bool operator==(const Token& other) const        // 相等比较
bool operator!=(const Token& other) const        // 不等比较
```
