#include "UnicodeCategories.h"

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {

    bool UnicodeCategories::isIdentifierStart(uint32_t codepoint) {
        // 首先验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return false;
        }

        // 快速路径：ASCII标识符开始字符
        // 使用查找表来加速ASCII范围的检查
        static const bool asciiIdentifierStart[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00-0x0F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40-0x4F (@, A-O)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  // 0x50-0x5F (P-Z, _, underscore)
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60-0x6F (`, a-o)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0   // 0x70-0x7F (p-z)
        };

        if (codepoint < 128) {
            return asciiIdentifierStart[codepoint];
        }

        // 检查Unicode类别
        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            // 检查是否属于标识符开始允许的类别
            switch (category) {
                case Category::Letter_Uppercase:
                case Category::Letter_Lowercase:
                case Category::Letter_Titlecase:
                case Category::Letter_Modifier:
                case Category::Letter_Other:
                    return true;
                default:
                    return false;
            }
        }

        return false;
    }

    bool UnicodeCategories::isIdentifierContinue(uint32_t codepoint) {
        // 首先验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return false;
        }

        // 快速路径：ASCII字符
        // 使用查找表来加速ASCII范围的检查
        static const bool asciiIdentifierContinue[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00-0x0F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F (0-9)
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40-0x4F (@, A-O)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  // 0x50-0x5F (P-Z, _, underscore)
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60-0x6F (`, a-o)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0   // 0x70-0x7F (p-z)
        };

        if (codepoint < 128) {
            return asciiIdentifierContinue[codepoint];
        }

        // 检查Unicode类别
        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            // 检查是否属于标识符继续允许的类别
            switch (category) {
                case Category::Number_Decimal:
                case Category::Number_Letter:
                case Category::Number_Other:
                case Category::Mark_NonSpacing:
                case Category::Mark_SpacingCombining:
                case Category::Mark_Enclosing:
                case Category::Punctuation_Connector:
                    return true;
                default:
                    // 如果不是上述类别，检查是否是合法的标识符开始字符
                    return isIdentifierStart(codepoint);
            }
        }

        return false;
    }

    bool UnicodeCategories::isWhitespace(uint32_t codepoint) {
        // 快速路径：ASCII空白字符
        static const bool asciiWhitespace[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,  // 0x00-0x0F (TAB, LF, VT, FF, CR)
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F (SPACE)
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 其余位置都是0
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        if (codepoint < 128) {
            return asciiWhitespace[codepoint];
        }

        // Unicode空白字符查找表
        // 使用二分查找优化大量case的查找
        static const uint32_t unicodeWhitespace[] = {
            0x00A0,  // NO-BREAK SPACE
            0x1680,  // OGHAM SPACE MARK
            0x2000,  // EN QUAD
            0x2001,  // EM QUAD
            0x2002,  // EN SPACE
            0x2003,  // EM SPACE
            0x2004,  // THREE-PER-EM SPACE
            0x2005,  // FOUR-PER-EM SPACE
            0x2006,  // SIX-PER-EM SPACE
            0x2007,  // FIGURE SPACE
            0x2008,  // PUNCTUATION SPACE
            0x2009,  // THIN SPACE
            0x200A,  // HAIR SPACE
            0x2028,  // LINE SEPARATOR
            0x2029,  // PARAGRAPH SEPARATOR
            0x202F,  // NARROW NO-BREAK SPACE
            0x205F,  // MEDIUM MATHEMATICAL SPACE
            0x3000   // IDEOGRAPHIC SPACE
        };

        // 二分查找Unicode空白字符
        int left = 0;
        int right = sizeof(unicodeWhitespace) / sizeof(uint32_t) - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (unicodeWhitespace[mid] == codepoint) {
                return true;
            }
            if (unicodeWhitespace[mid] < codepoint) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        return false;
    }

    bool UnicodeCategories::isDigit(uint32_t codepoint) {
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return false;
        }

        // ASCII数字快速路径
        static const bool asciiDigit[128] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00-0x0F
                                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
                                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F (0-9)
                                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 其余位置都是0
                                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        if (codepoint < 128) {
            return asciiDigit[codepoint];
        }

        // 检查是否为Unicode十进制数字
        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            // 包括扩展的数字类别
            switch (category) {
                case Category::Number_Decimal:  // 十进制数字
                case Category::Number_Letter:   // 字母数字（如罗马数字）
                case Category::Number_Other:    // 其他数字
                    return true;
                default:
                    return false;
            }
        }

        return false;
    }

    bool UnicodeCategories::isHexDigit(uint32_t codepoint) {
        // 使用查找表优化十六进制数字检查
        static const bool asciiHexDigit[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00-0x0F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F (0-9)
            0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x40-0x4F (A-F)
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x50-0x5F
            0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x60-0x6F (a-f)
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // 0x70-0x7F
        };

        return codepoint < 128 && asciiHexDigit[codepoint];
    }

    bool UnicodeCategories::isAlpha(uint32_t codepoint) {
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return false;
        }

        // ASCII字母快速路径
        static const bool asciiAlpha[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00-0x0F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40-0x4F (@, A-O)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 0x50-0x5F (P-Z)
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60-0x6F (`, a-o)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0   // 0x70-0x7F (p-z)
        };

        if (codepoint < 128) {
            return asciiAlpha[codepoint];
        }

        // 检查Unicode字母类别
        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            switch (category) {
                case Category::Letter_Uppercase:
                case Category::Letter_Lowercase:
                case Category::Letter_Titlecase:
                case Category::Letter_Modifier:
                case Category::Letter_Other:
                    return true;
                default:
                    return false;
            }
        }

        return false;
    }

    bool UnicodeCategories::isAlnum(uint32_t codepoint) {
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return false;
        }

        // ASCII字母数字快速路径
        static const bool asciiAlnum[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00-0x0F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F (0-9)
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40-0x4F (@, A-O)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 0x50-0x5F (P-Z)
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60-0x6F (`, a-o)
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0   // 0x70-0x7F (p-z)
        };

        if (codepoint < 128) {
            return asciiAlnum[codepoint];
        }

        return isAlpha(codepoint) || isDigit(codepoint);
    }

    UnicodeCategories::Category UnicodeCategories::getCategory(uint32_t codepoint) {
        // 首先验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return Category::Other_NotAssigned;
        }

        // ASCII字符快速路径
        if (codepoint < 128) {
            // ASCII控制字符
            if (codepoint < 0x20 || codepoint == 0x7F) {
                return Category::Other_Control;
            }
            // ASCII空格
            if (codepoint == 0x20) {
                return Category::Separator_Space;
            }
            // ASCII数字
            if (codepoint >= '0' && codepoint <= '9') {
                return Category::Number_Decimal;
            }
            // ASCII大写字母
            if (codepoint >= 'A' && codepoint <= 'Z') {
                return Category::Letter_Uppercase;
            }
            // ASCII小写字母
            if (codepoint >= 'a' && codepoint <= 'z') {
                return Category::Letter_Lowercase;
            }
            // ASCII标点符号
            return Category::Punctuation_Other;
        }

        // 代理项范围快速检查
        if (codepoint >= SURROGATE_MIN && codepoint <= SURROGATE_MAX) {
            return Category::Other_Surrogate;
        }

        // 私有使用区域快速检查
        if (codepoint >= 0xE000 && codepoint <= 0xF8FF) {
            return Category::Other_PrivateUse;
        }

        Category category;
        if (binarySearchUnicodeData(codepoint, category)) {
            return category;
        }

        // 未分配的码点
        return Category::Other_NotAssigned;
    }

    bool UnicodeCategories::isInCategory(uint32_t codepoint, Category category) {
        // 优化：对于某些类别，可以进行快速路径检查
        switch (category) {
            case Category::Letter_Uppercase:
                if (codepoint >= 'A' && codepoint <= 'Z') {
                    return true;
                }
                break;
            case Category::Letter_Lowercase:
                if (codepoint >= 'a' && codepoint <= 'z') {
                    return true;
                }
                break;
            case Category::Number_Decimal:
                if (codepoint >= '0' && codepoint <= '9') {
                    return true;
                }
                break;
            case Category::Separator_Space:
                if (codepoint == ' ' || codepoint == '\t') {
                    return true;
                }
                break;
            case Category::Other_Control:
                if (codepoint < 0x20 || codepoint == 0x7F) {
                    return true;
                }
                break;
            default:
                break;
        }

        // 如果快速路径未命中，则进行完整的类别检查
        return getCategory(codepoint) == category;
    }

    // Unicode字符分类数据
    bool UnicodeCategories::binarySearchUnicodeData(uint32_t codepoint, Category &category) {
        // 使用二分查找在Unicode范围表中查找码点
        // 表已按起始码点排序，可以进行高效的二分查找
        const size_t rangeCount = sizeof(unicodeRanges) / sizeof(UnicodeRange);
        size_t left = 0;
        size_t right = rangeCount - 1;

        while (left <= right) {
            size_t mid = left + (right - left) / 2;
            const UnicodeRange &range = unicodeRanges[mid];

            // 检查码点是否在当前范围内
            if (codepoint < range.start) {
                if (mid == 0) break;  // 防止无符号整数下溢
                right = mid - 1;
            } else if (codepoint > range.end) {
                left = mid + 1;
            } else {
                // 找到匹配的范围
                category = range.category;
                return true;
            }
        }

        // 未找到匹配的范围
        category = Category::Other_NotAssigned;
        return false;
    }

}  // namespace rp::frontend::unicode
