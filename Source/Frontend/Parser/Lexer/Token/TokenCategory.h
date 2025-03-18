#pragma once

namespace rp {
    namespace frontend {

        // Token类别
        enum class TokenCategory {
            Special,     // 特殊token（EOF, Invalid等）
            Literal,     // 字面量
            Identifier,  // 标识符
            Keyword,     // 关键字
            Operator,    // 运算符
            Delimiter,   // 分隔符
            Attribute    // 属性说明符
        };

    }  // namespace frontend
}  // namespace rp
