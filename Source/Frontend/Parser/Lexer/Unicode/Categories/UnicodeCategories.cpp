#include "UnicodeCategories.h"

namespace rp::frontend::unicode {

    bool UnicodeCategories::isIdentifierStart(uint32_t codepoint) {
        // 基本的ASCII标识符开始字符规则
        if ((codepoint >= 'a' && codepoint <= 'z') || (codepoint >= 'A' && codepoint <= 'Z') || codepoint == '_') {
            return true;
        }

        // 检查Unicode类别
        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            return category == Category::Letter_Uppercase || category == Category::Letter_Lowercase ||
                   category == Category::Letter_Titlecase || category == Category::Letter_Modifier ||
                   category == Category::Letter_Other;
        }

        return false;
    }

    bool UnicodeCategories::isIdentifierContinue(uint32_t codepoint) {
        // 如果是标识符开始字符，也可以作为继续字符
        if (isIdentifierStart(codepoint)) {
            return true;
        }

        // 基本的ASCII数字
        if (codepoint >= '0' && codepoint <= '9') {
            return true;
        }

        // 检查Unicode类别
        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            return category == Category::Number_Decimal || category == Category::Number_Letter ||
                   category == Category::Number_Other || category == Category::Mark_NonSpacing ||
                   category == Category::Mark_SpacingCombining || category == Category::Mark_Enclosing ||
                   category == Category::Punctuation_Connector;
        }

        return false;
    }

    bool UnicodeCategories::isWhitespace(uint32_t codepoint) {
        // 基本的ASCII空白字符规则
        return (codepoint == ' ' || codepoint == '\t' || codepoint == '\n' || codepoint == '\r');
    }

    bool UnicodeCategories::isDigit(uint32_t codepoint) { return codepoint >= '0' && codepoint <= '9'; }

    bool UnicodeCategories::isHexDigit(uint32_t codepoint) {
        return (codepoint >= '0' && codepoint <= '9') || (codepoint >= 'a' && codepoint <= 'f') ||
               (codepoint >= 'A' && codepoint <= 'F');
    }

    bool UnicodeCategories::isAlpha(uint32_t codepoint) {
        return (codepoint >= 'a' && codepoint <= 'z') || (codepoint >= 'A' && codepoint <= 'Z');
    }

    bool UnicodeCategories::isAlnum(uint32_t codepoint) { return isAlpha(codepoint) || isDigit(codepoint); }

    UnicodeCategories::Category UnicodeCategories::getCategory(uint32_t codepoint) {
        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            return category;
        }
        return Category::Other_NotAssigned;
    }

    bool UnicodeCategories::isInCategory(uint32_t codepoint, Category category) {
        return getCategory(codepoint) == category;
    }

    // Unicode字符分类数据表
    struct UnicodeRange {
        uint32_t start;
        uint32_t end;
        UnicodeCategories::Category category;
    };

    // 部分常用Unicode范围的分类数据
    static const UnicodeRange unicodeRanges[] = {
        {0x0000, 0x001F, UnicodeCategories::Category::Other_Control},        // C0控制字符
        {0x0020, 0x0020, UnicodeCategories::Category::Separator_Space},      // 空格
        {0x0021, 0x002F, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
        {0x0030, 0x0039, UnicodeCategories::Category::Number_Decimal},       // ASCII数字
        {0x003A, 0x0040, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
        {0x0041, 0x005A, UnicodeCategories::Category::Letter_Uppercase},     // ASCII大写字母
        {0x005B, 0x0060, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
        {0x0061, 0x007A, UnicodeCategories::Category::Letter_Lowercase},     // ASCII小写字母
        {0x007B, 0x007E, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
        {0x007F, 0x009F, UnicodeCategories::Category::Other_Control},        // C1控制字符
        {0x00A0, 0x00A0, UnicodeCategories::Category::Separator_Space},      // 不间断空格
        {0x00A1, 0x00BF, UnicodeCategories::Category::Punctuation_Other},    // Latin-1标点
        {0x00C0, 0x00D6, UnicodeCategories::Category::Letter_Uppercase},     // Latin-1大写字母
        {0x00D7, 0x00D7, UnicodeCategories::Category::Symbol_Math},          // 乘号
        {0x00D8, 0x00DE, UnicodeCategories::Category::Letter_Uppercase},     // Latin-1大写字母
        {0x00DF, 0x00F6, UnicodeCategories::Category::Letter_Lowercase},     // Latin-1小写字母
        {0x00F7, 0x00F7, UnicodeCategories::Category::Symbol_Math},          // 除号
        {0x00F8, 0x00FF, UnicodeCategories::Category::Letter_Lowercase},     // Latin-1小写字母
        {0x0100, 0x017F, UnicodeCategories::Category::Letter_Other},         // 扩展Latin-A
        {0x2000, 0x200A, UnicodeCategories::Category::Separator_Space},      // 各种空格
        {0x2028, 0x2028, UnicodeCategories::Category::Separator_Line},       // 行分隔符
        {0x2029, 0x2029, UnicodeCategories::Category::Separator_Paragraph},  // 段落分隔符
        {0x3000, 0x303F, UnicodeCategories::Category::Punctuation_Other},    // CJK符号和标点
        {0x3040, 0x309F, UnicodeCategories::Category::Letter_Other},         // 平假名
        {0x30A0, 0x30FF, UnicodeCategories::Category::Letter_Other},         // 片假名
        {0x3400, 0x4DBF, UnicodeCategories::Category::Letter_Other},         // CJK统一表意文字扩展A
        {0x4E00, 0x9FFF, UnicodeCategories::Category::Letter_Other},         // CJK统一表意文字
        {0xAC00, 0xD7AF, UnicodeCategories::Category::Letter_Other},         // 朝鲜文音节
        {0xD800, 0xDBFF, UnicodeCategories::Category::Other_Surrogate},      // 高位代理项
        {0xDC00, 0xDFFF, UnicodeCategories::Category::Other_Surrogate},      // 低位代理项
        {0xE000, 0xF8FF, UnicodeCategories::Category::Other_PrivateUse},     // 私用区
        {0xF900, 0xFAFF, UnicodeCategories::Category::Letter_Other},         // CJK兼容表意文字
        {0xFB00, 0xFB4F, UnicodeCategories::Category::Letter_Other},         // 字母表示形式
        {0xFE00, 0xFE0F, UnicodeCategories::Category::Mark_NonSpacing},      // 变体选择符
        {0xFFF0, 0xFFFF, UnicodeCategories::Category::Other_NotAssigned}     // 特殊
    };

    bool UnicodeCategories::binarySearchUnicodeData(uint32_t codepoint, Category &category) {
        int left = 0;
        int right = sizeof(unicodeRanges) / sizeof(UnicodeRange) - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            const UnicodeRange &range = unicodeRanges[mid];

            if (codepoint < range.start) {
                right = mid - 1;
            } else if (codepoint > range.end) {
                left = mid + 1;
            } else {
                category = range.category;
                return true;
            }
        }

        // 如果没有找到匹配的范围，将其归类为未分配
        category = Category::Other_NotAssigned;
        return false;
    }

}  // namespace rp::frontend::unicode
