
#pragma once

namespace rp {
    namespace frontend {

        enum class TokenKind {
// 使用X-Macro技术自动生成枚举值
#define TOKEN(name, str) name,
#define KEYWORD(name, str) Keyword_##name,
#define OPERATOR(name, str) name,
#define DELIMITER(name, str) name,
#define DIRECTIVE(name, str) Directive_##name,
#define TYPE(name, str) Keyword_##name,
#include "TokenKind.def"
#undef TOKEN
#undef KEYWORD
#undef OPERATOR
#undef DELIMITER
#undef DIRECTIVE
#undef TYPE
        };

        // 辅助函数声明
        bool isKeyword(TokenKind kind);
        bool isOperator(TokenKind kind);
        bool isDelimiter(TokenKind kind);
        bool isDirective(TokenKind kind);
        bool isTypeKeyword(TokenKind kind);

        // 获取TokenKind对应的字符串表示
        const char* getTokenKindName(TokenKind kind);

    }  // namespace frontend
}  // namespace rp
