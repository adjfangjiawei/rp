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

            // 基础Token范围标记
            FirstBaseToken,
#include "TokenDefs/BaseTokens.def"
            LastBaseToken,

            // 字符串字面量范围标记
            FirstStringLiteral,
#include "TokenDefs/StringLiterals.def"
            LastStringLiteral,

            // 关键字范围标记
            FirstKeyword,
#include "TokenDefs/Keywords.def"
#include "TokenDefs/Types.def"
            LastKeyword,

            // 运算符范围标记
            FirstOperator,
#include "TokenDefs/Operators.def"
            LastOperator,

            // 分隔符范围标记
            FirstDelimiter,
#include "TokenDefs/Delimiters.def"
            LastDelimiter,

            // 预处理指令范围标记
            FirstDirective,
#include "TokenDefs/Directives.def"
            LastDirective

#undef TOKEN
#undef KEYWORD
#undef OPERATOR
#undef DELIMITER
#undef DIRECTIVE
#undef TYPE
        };

    }  // namespace frontend
}  // namespace rp
