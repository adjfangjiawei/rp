#pragma once
#include <string_view>

namespace rp {
    namespace frontend {

        /**
         * Token的主要分类
         * 用于在词法分析和语法分析阶段快速判断Token的类型
         */
        enum class TokenCategory {
            Special,     // 特殊token（EOF, Invalid等）和预处理指令
            Literal,     // 字面量（数字、字符串、字符等）
            Identifier,  // 标识符
            Keyword,     // 关键字（包括类型关键字）
            Operator,    // 运算符
            Delimiter,   // 分隔符（括号、逗号、分号等）
            Attribute    // 属性说明符（[[...]]）
        };

        /**
         * 获取TokenCategory的字符串表示
         */
        constexpr std::string_view getTokenCategoryName(TokenCategory category) {
            switch (category) {
                case TokenCategory::Special:
                    return "Special";
                case TokenCategory::Literal:
                    return "Literal";
                case TokenCategory::Identifier:
                    return "Identifier";
                case TokenCategory::Keyword:
                    return "Keyword";
                case TokenCategory::Operator:
                    return "Operator";
                case TokenCategory::Delimiter:
                    return "Delimiter";
                case TokenCategory::Attribute:
                    return "Attribute";
                default:
                    return "Unknown";
            }
        }

        /**
         * 检查TokenCategory是否表示语言关键元素
         */
        constexpr bool isKeywordCategory(TokenCategory category) { return category == TokenCategory::Keyword; }

        /**
         * 检查TokenCategory是否表示操作符
         */
        constexpr bool isOperatorCategory(TokenCategory category) { return category == TokenCategory::Operator; }

        /**
         * 检查TokenCategory是否表示字面量
         */
        constexpr bool isLiteralCategory(TokenCategory category) { return category == TokenCategory::Literal; }

        /**
         * 检查TokenCategory是否表示标识符
         */
        constexpr bool isIdentifierCategory(TokenCategory category) { return category == TokenCategory::Identifier; }

        /**
         * 检查TokenCategory是否表示分隔符
         */
        constexpr bool isDelimiterCategory(TokenCategory category) { return category == TokenCategory::Delimiter; }

        /**
         * 检查TokenCategory是否表示特殊Token
         */
        constexpr bool isSpecialCategory(TokenCategory category) { return category == TokenCategory::Special; }

        /**
         * 检查TokenCategory是否表示属性
         */
        constexpr bool isAttributeCategory(TokenCategory category) { return category == TokenCategory::Attribute; }

    }  // namespace frontend
}  // namespace rp
