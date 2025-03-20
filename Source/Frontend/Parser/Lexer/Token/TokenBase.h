#pragma once
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"
#include "TokenCategory.h"
#include "TokenKind.h"

namespace rp {
    namespace frontend {

        // Token基础结构体
        struct Token {
            TokenKind kind;
            std::string textStorage;  // 存储实际的字符串内容
            std::string_view text;    // 指向源码中的原始文本或textStorage
            unsigned line;
            unsigned column;
            std::string filename;

            // 字面量值
            union {
                long long intValue;
                double floatValue;
                bool boolValue;
            };

            // 错误恢复信息
            struct ErrorInfo {
                bool hasError{false};
                std::string message;
                unsigned errorLine{0};
                unsigned errorColumn{0};
            };
            std::shared_ptr<ErrorInfo> errorInfo;

            // 字符串字面量的额外信息
            struct StringInfo {
                bool isRawString{false};
                std::string delimiter;  // 原始字符串的分隔符
                bool isWide{false};     // 是否是宽字符串
                bool isUTF8{false};     // 是否是UTF-8字符串
                bool isUTF16{false};    // 是否是UTF-16字符串
                bool isUTF32{false};    // 是否是UTF-32字符串
            };
            std::shared_ptr<StringInfo> stringInfo;

            // 默认构造函数
            Token() : kind(TokenKind::Invalid), line(0), column(0), intValue(0) {}

            // 基本构造函数
            explicit Token(TokenKind k) : kind(k), line(0), column(0), intValue(0) {}

            // 位置信息构造函数
            Token(TokenKind k, unsigned ln, unsigned col) : kind(k), line(ln), column(col), intValue(0) {}

            // 带位置的构造函数
            Token(TokenKind k, size_t pos, unsigned ln, unsigned col) : kind(k), line(ln), column(col), intValue(0) {}

            // 完整构造函数
            Token(TokenKind k, unsigned ln, unsigned col, const std::string& fname)
                : kind(k), line(ln), column(col), filename(fname), intValue(0) {}

            // 带位置和值的构造函数
            Token(TokenKind k, size_t pos, unsigned ln, unsigned col, const NumberValue& value)
                : kind(k), line(ln), column(col), intValue(0) {
                if (value.kind == NumberKind::Integer) {
                    intValue = value.value;
                } else {
                    floatValue = value.value;
                }
            }

            // 数值构造函数
            Token(TokenKind k, unsigned ln, unsigned col, const NumberValue& value)
                : kind(k), line(ln), column(col), intValue(0) {
                if (value.kind == NumberKind::Integer) {
                    intValue = value.value;
                } else {
                    floatValue = value.value;
                }
            }

            // 设置文本内容
            void setText(std::string_view sv) {
                textStorage = std::string(sv);
                text = textStorage;
            }

            void setText(std::string&& str) {
                textStorage = std::move(str);
                text = textStorage;
            }

            void setText(const std::string& str) {
                textStorage = str;
                text = textStorage;
            }

            // 设置错误信息
            void setError(const std::string& message, unsigned errorLine = 0, unsigned errorColumn = 0) {
                if (!errorInfo) {
                    errorInfo = std::make_shared<ErrorInfo>();
                }
                errorInfo->hasError = true;
                errorInfo->message = message;
                errorInfo->errorLine = errorLine ? errorLine : line;
                errorInfo->errorColumn = errorColumn ? errorColumn : column;
            }

            // 设置字符串信息
            void setStringInfo(bool isRaw = false, const std::string& delim = "") {
                if (!stringInfo) {
                    stringInfo = std::make_shared<StringInfo>();
                }
                stringInfo->isRawString = isRaw;
                stringInfo->delimiter = delim;
            }

            // 设置字符串编码类型
            void setStringEncoding(bool isWide = false,
                                   bool isUTF8 = false,
                                   bool isUTF16 = false,
                                   bool isUTF32 = false) {
                if (!stringInfo) {
                    stringInfo = std::make_shared<StringInfo>();
                }
                stringInfo->isWide = isWide;
                stringInfo->isUTF8 = isUTF8;
                stringInfo->isUTF16 = isUTF16;
                stringInfo->isUTF32 = isUTF32;
            }

            // 检查是否有错误
            bool hasError() const { return errorInfo && errorInfo->hasError; }

            // 获取错误信息
            std::string getErrorMessage() const { return errorInfo ? errorInfo->message : ""; }

            // 获取错误位置
            std::optional<std::pair<unsigned, unsigned>> getErrorLocation() const {
                if (errorInfo && errorInfo->hasError) {
                    return std::make_pair(errorInfo->errorLine, errorInfo->errorColumn);
                }
                return std::nullopt;
            }

            // 检查字符串类型
            bool isRawString() const { return stringInfo && stringInfo->isRawString; }
            bool isWideString() const { return stringInfo && stringInfo->isWide; }
            bool isUTF8String() const { return stringInfo && stringInfo->isUTF8; }
            bool isUTF16String() const { return stringInfo && stringInfo->isUTF16; }
            bool isUTF32String() const { return stringInfo && stringInfo->isUTF32; }

            // 获取原始字符串分隔符
            std::string getDelimiter() const { return stringInfo ? stringInfo->delimiter : ""; }

            // 获取完整的位置信息
            std::string getLocation() const {
                return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
            }

            // 类型安全的值获取函数
            std::optional<long long> getIntValue() const {
                if (kind == TokenKind::NumberLiteral) {
                    return intValue;
                }
                return std::nullopt;
            }

            std::optional<double> getFloatValue() const {
                if (kind == TokenKind::NumberLiteral) {
                    return floatValue;
                }
                return std::nullopt;
            }

            std::optional<bool> getBoolValue() const {
                if (kind == TokenKind::NumberLiteral) {
                    return boolValue;
                }
                return std::nullopt;
            }

            // 拷贝构造函数
            Token(const Token& other)
                : kind(other.kind),
                  textStorage(other.textStorage),
                  line(other.line),
                  column(other.column),
                  filename(other.filename),
                  intValue(other.intValue),
                  errorInfo(other.errorInfo),
                  stringInfo(other.stringInfo) {
                text = textStorage;
            }

            // 拷贝赋值操作符
            Token& operator=(const Token& other) {
                if (this != &other) {
                    kind = other.kind;
                    textStorage = other.textStorage;
                    text = textStorage;
                    line = other.line;
                    column = other.column;
                    filename = other.filename;
                    intValue = other.intValue;
                    errorInfo = other.errorInfo;
                    stringInfo = other.stringInfo;
                }
                return *this;
            }

            // 移动构造函数
            Token(Token&& other) noexcept
                : kind(other.kind),
                  textStorage(std::move(other.textStorage)),
                  line(other.line),
                  column(other.column),
                  filename(std::move(other.filename)),
                  intValue(other.intValue),
                  errorInfo(std::move(other.errorInfo)),
                  stringInfo(std::move(other.stringInfo)) {
                text = textStorage;
            }

            // 移动赋值操作符
            Token& operator=(Token&& other) noexcept {
                if (this != &other) {
                    kind = other.kind;
                    textStorage = std::move(other.textStorage);
                    text = textStorage;
                    line = other.line;
                    column = other.column;
                    filename = std::move(other.filename);
                    intValue = other.intValue;
                    errorInfo = std::move(other.errorInfo);
                    stringInfo = std::move(other.stringInfo);
                }
                return *this;
            }

            // Token类型判断函数
            inline bool isKeyword() const;
            inline bool isOperator() const;
            inline bool isDelimiter() const;

            // Getter methods
            std::string_view getText() const { return text; }
            TokenKind getKind() const { return kind; }
            unsigned getLine() const { return line; }
            unsigned getColumn() const { return column; }
            const std::string& getFilename() const { return filename; }

            // 比较操作符
            bool operator==(const Token& other) const { return kind == other.kind && text == other.text; }

            bool operator!=(const Token& other) const { return !(*this == other); }
        };

    }  // namespace frontend
}  // namespace rp
