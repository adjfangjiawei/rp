
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

    }  // namespace frontend
}  // namespace rp
