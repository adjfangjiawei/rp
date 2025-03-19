
#pragma once
#include <cstdint>
#include <string_view>

namespace rp::frontend::unicode {

    class UnicodeCategories {
      public:
        enum class Category {
            Letter_Uppercase,
            Letter_Lowercase,
            Letter_Titlecase,
            Letter_Modifier,
            Letter_Other,
            Number_Decimal,
            Number_Letter,
            Number_Other,
            Punctuation_Connector,
            Punctuation_Dash,
            Punctuation_Open,
            Punctuation_Close,
            Punctuation_Quote,
            Punctuation_Other,
            Symbol_Math,
            Symbol_Currency,
            Symbol_Modifier,
            Symbol_Other,
            Separator_Space,
            Separator_Line,
            Separator_Paragraph,
            Mark_NonSpacing,
            Mark_SpacingCombining,
            Mark_Enclosing,
            Other_Control,
            Other_Format,
            Other_Surrogate,
            Other_PrivateUse,
            Other_NotAssigned
        };

        // Unicode属性检查
        static bool isIdentifierStart(uint32_t codepoint);
        static bool isIdentifierContinue(uint32_t codepoint);
        static bool isWhitespace(uint32_t codepoint);
        static bool isDigit(uint32_t codepoint);
        static bool isHexDigit(uint32_t codepoint);
        static bool isAlpha(uint32_t codepoint);
        static bool isAlnum(uint32_t codepoint);

        // Unicode字符分类
        static Category getCategory(uint32_t codepoint);
        static bool isInCategory(uint32_t codepoint, Category category);

      private:
        static bool binarySearchUnicodeData(uint32_t codepoint, Category &category);
    };

}  // namespace rp::frontend::unicode
