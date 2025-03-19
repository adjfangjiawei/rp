#pragma once
#include <memory>
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

            // 拷贝构造函数
            Token(const Token& other)
                : kind(other.kind),
                  textStorage(other.textStorage),
                  line(other.line),
                  column(other.column),
                  filename(other.filename),
                  intValue(other.intValue) {
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
                  intValue(other.intValue) {
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
        };

    }  // namespace frontend
}  // namespace rp
