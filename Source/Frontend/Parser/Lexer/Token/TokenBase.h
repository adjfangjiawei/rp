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
            std::string_view text;  // 指向源码中的原始文本
            unsigned line;
            unsigned column;
            std::string filename;

            // 字面量值
            union {
                long long intValue;
                double floatValue;
                bool boolValue;
            };

            // 构造函数
            Token(TokenKind k = TokenKind::Invalid) : kind(k), line(0), column(0), intValue(0) {}

            Token(TokenKind k, size_t pos, size_t ln, size_t col) : kind(k), line(ln), column(col), intValue(0) {}

            Token(TokenKind k, size_t pos, size_t ln, size_t col, const NumberValue& value)
                : kind(k), line(ln), column(col) {
                if (value.kind == NumberKind::Integer) {
                    intValue = value.value;
                } else {
                    floatValue = value.value;
                }
            }

            // 禁用拷贝构造和赋值操作符，因为union中的成员可能会导致问题
            Token(const Token&) = delete;
            Token& operator=(const Token&) = delete;

            // 允许移动构造和赋值
            Token(Token&&) = default;
            Token& operator=(Token&&) = default;

            // Token类型判断函数
            inline bool isKeyword() const;
            inline bool isOperator() const;
            inline bool isDelimiter() const;
        };

    }  // namespace frontend
}  // namespace rp
