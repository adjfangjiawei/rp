#include "Unicode.h"

#include <array>
#include <unordered_map>
#include <unordered_set>

namespace rp {
    namespace frontend {

        namespace {
            // Unicode字符宽度表
            struct UnicodeWidthEntry {
                uint32_t start;
                uint32_t end;
                int width;
            };

            // 东亚宽度字符范围表
            constexpr std::array<UnicodeWidthEntry, 10> widthTable = {{
                {0x1100, 0x115F, 2},  // Hangul Jamo
                {0x2E80, 0x2FFF, 2},  // CJK部首补充
                {0x3000, 0x303F, 2},  // CJK符号和标点
                {0x3040, 0x309F, 2},  // 平假名
                {0x30A0, 0x30FF, 2},  // 片假名
                {0x3100, 0x312F, 2},  // 注音符号
                {0x3130, 0x318F, 2},  // 谚文兼容字母
                {0x31C0, 0x31EF, 2},  // CJK笔画
                {0x31F0, 0x31FF, 2},  // 片假名语音扩展
                {0x4E00, 0x9FFF, 2},  // CJK统一表意文字
            }};

            // Unicode正规化映射表
            const std::unordered_map<uint32_t, uint32_t> decompositionMap = {
                // 添加常见的分解映射
                {0x00C0, 0x0041},  // À -> A
                {0x00E0, 0x0061},  // à -> a
                {0x00C1, 0x0041},  // Á -> A
                {0x00E1, 0x0061},  // á -> a
                // 可以继续添加更多映射...
            };

            // Unicode大小写映射表
            const std::unordered_map<uint32_t, uint32_t> upperCaseMap = {
                {0x0061, 0x0041},  // a -> A
                {0x0062, 0x0042},  // b -> B
                {0x0063, 0x0043},  // c -> C
                // 可以继续添加更多映射...
            };

            const std::unordered_map<uint32_t, uint32_t> lowerCaseMap = {
                {0x0041, 0x0061},  // A -> a
                {0x0042, 0x0062},  // B -> b
                {0x0043, 0x0063},  // C -> c
                // 可以继续添加更多映射...
            };
        }  // namespace

        // Unicode字符类别的实现
        bool Unicode::isIdentifierStart(uint32_t codepoint) {
            // 基本拉丁字母
            if ((codepoint >= 0x0041 && codepoint <= 0x005A) ||  // A-Z
                (codepoint >= 0x0061 && codepoint <= 0x007A) ||  // a-z
                codepoint == 0x005F) {                           // _
                return true;
            }

            // 扩展拉丁字母
            if (codepoint >= 0x00C0 && codepoint <= 0x00D6) return true;
            if (codepoint >= 0x00D8 && codepoint <= 0x00F6) return true;
            if (codepoint >= 0x00F8 && codepoint <= 0x00FF) return true;

            // 其他Unicode字母
            Category cat = getCategory(codepoint);
            return cat == Category::Letter_Uppercase || cat == Category::Letter_Lowercase ||
                   cat == Category::Letter_Titlecase || cat == Category::Letter_Modifier ||
                   cat == Category::Letter_Other;
        }

        bool Unicode::isIdentifierStart(std::string_view sv) {
            if (sv.empty()) return false;

            size_t bytesRead;
            uint32_t codePoint = utf8ToCodePoint(sv, bytesRead);
            return isIdentifierStart(codePoint);
        }

        bool Unicode::isIdentifierContinue(uint32_t codepoint) {
            if (isIdentifierStart(codepoint)) return true;

            // 数字
            if (codepoint >= 0x0030 && codepoint <= 0x0039) return true;

            // 其他Unicode数字和连接符号
            Category cat = getCategory(codepoint);
            return cat == Category::Number_Decimal || cat == Category::Number_Letter || cat == Category::Number_Other ||
                   cat == Category::Mark_NonSpacing || cat == Category::Mark_SpacingCombining;
        }

        bool Unicode::isIdentifierContinue(std::string_view sv) {
            if (sv.empty()) return false;

            size_t bytesRead;
            uint32_t codePoint = utf8ToCodePoint(sv, bytesRead);
            return isIdentifierContinue(codePoint);
        }

        bool Unicode::isDigit(uint32_t codepoint) {
            return (codepoint >= 0x0030 && codepoint <= 0x0039) || getCategory(codepoint) == Category::Number_Decimal;
        }

        bool Unicode::isHexDigit(uint32_t codepoint) {
            return (codepoint >= 0x0030 && codepoint <= 0x0039) ||  // 0-9
                   (codepoint >= 0x0041 && codepoint <= 0x0046) ||  // A-F
                   (codepoint >= 0x0061 && codepoint <= 0x0066);    // a-f
        }

        bool Unicode::isAlpha(uint32_t codepoint) {
            Category cat = getCategory(codepoint);
            return cat == Category::Letter_Uppercase || cat == Category::Letter_Lowercase ||
                   cat == Category::Letter_Titlecase || cat == Category::Letter_Modifier ||
                   cat == Category::Letter_Other;
        }

        bool Unicode::isAlnum(uint32_t codepoint) { return isAlpha(codepoint) || isDigit(codepoint); }

        Unicode::Category Unicode::getCategory(uint32_t codepoint) {
            // 基本拉丁字母
            if (codepoint >= 0x0041 && codepoint <= 0x005A)  // A-Z
                return Category::Letter_Uppercase;
            if (codepoint >= 0x0061 && codepoint <= 0x007A)  // a-z
                return Category::Letter_Lowercase;

            // 数字
            if (codepoint >= 0x0030 && codepoint <= 0x0039)  // 0-9
                return Category::Number_Decimal;

            // ASCII标点符号
            if (codepoint <= 0x007F) {
                if (codepoint >= 0x0020 && codepoint <= 0x002F) return Category::Punctuation_Other;
                if (codepoint >= 0x003A && codepoint <= 0x0040) return Category::Punctuation_Other;
                if (codepoint >= 0x005B && codepoint <= 0x0060) return Category::Punctuation_Other;
                if (codepoint >= 0x007B && codepoint <= 0x007E) return Category::Punctuation_Other;
            }

            // 空白字符
            if (isWhitespace(codepoint)) return Category::Separator_Space;

            // 扩展拉丁字母
            if ((codepoint >= 0x00C0 && codepoint <= 0x00D6) || (codepoint >= 0x00D8 && codepoint <= 0x00DE))
                return Category::Letter_Uppercase;
            if ((codepoint >= 0x00DF && codepoint <= 0x00F6) || (codepoint >= 0x00F8 && codepoint <= 0x00FF))
                return Category::Letter_Lowercase;

            // 其他Unicode类别
            // TODO: 实现完整的Unicode类别查找表
            return Category::Other_NotAssigned;
        }

        bool Unicode::isWhitespace(uint32_t codepoint) {
            static const std::unordered_set<uint32_t> whitespaceSet = {
                0x0009,  // 水平制表符
                0x000A,  // 换行
                0x000B,  // 垂直制表符
                0x000C,  // 换页
                0x000D,  // 回车
                0x0020,  // 空格
                0x0085,  // 下一行
                0x00A0,  // 不换行空格
                0x2000,  // En Quad
                0x2001,  // Em Quad
                0x2002,  // En Space
                0x2003,  // Em Space
                0x2004,  // Three-Per-Em Space
                0x2005,  // Four-Per-Em Space
                0x2006,  // Six-Per-Em Space
                0x2007,  // Figure Space
                0x2008,  // Punctuation Space
                0x2009,  // Thin Space
                0x200A,  // Hair Space
                0x2028,  // Line Separator
                0x2029,  // Paragraph Separator
                0x202F,  // Narrow No-Break Space
                0x205F,  // Medium Mathematical Space
                0x3000   // Ideographic Space
            };

            return whitespaceSet.find(codepoint) != whitespaceSet.end();
        }

        std::string Unicode::normalize(const std::string& str, bool compose) {
            std::vector<uint32_t> codepoints;
            size_t pos = 0;
            while (pos < str.length()) {
                size_t bytesRead;
                uint32_t cp = utf8ToCodePoint(std::string_view(str.data() + pos, str.length() - pos), bytesRead);
                if (bytesRead == 0) break;

                // 分解
                auto it = decompositionMap.find(cp);
                if (it != decompositionMap.end()) {
                    cp = it->second;
                }

                codepoints.push_back(cp);
                pos += bytesRead;
            }

            // 重组（如果需要）
            if (compose) {
                // TODO: 实现重组逻辑
            }

            // 转换回UTF-8
            std::string result;
            for (uint32_t cp : codepoints) {
                result += codePointToUtf8(cp);
            }
            return result;
        }

        std::string Unicode::toUpper(const std::string& str) {
            std::string result;
            size_t pos = 0;
            while (pos < str.length()) {
                size_t bytesRead;
                uint32_t cp = utf8ToCodePoint(std::string_view(str.data() + pos, str.length() - pos), bytesRead);
                if (bytesRead == 0) break;

                auto it = upperCaseMap.find(cp);
                if (it != upperCaseMap.end()) {
                    cp = it->second;
                }

                result += codePointToUtf8(cp);
                pos += bytesRead;
            }
            return result;
        }

        std::string Unicode::toLower(const std::string& str) {
            std::string result;
            size_t pos = 0;
            while (pos < str.length()) {
                size_t bytesRead;
                uint32_t cp = utf8ToCodePoint(std::string_view(str.data() + pos, str.length() - pos), bytesRead);
                if (bytesRead == 0) break;

                auto it = lowerCaseMap.find(cp);
                if (it != lowerCaseMap.end()) {
                    cp = it->second;
                }

                result += codePointToUtf8(cp);
                pos += bytesRead;
            }
            return result;
        }

        int Unicode::getCharWidth(uint32_t codepoint) {
            // 检查特殊范围
            for (const auto& entry : widthTable) {
                if (codepoint >= entry.start && codepoint <= entry.end) {
                    return entry.width;
                }
            }

            // 控制字符
            if (codepoint < 0x20 || codepoint == 0x7F) {
                return 0;
            }

            // 默认宽度
            return 1;
        }

        size_t Unicode::utf8Length(const std::string& str) {
            size_t length = 0;
            size_t pos = 0;
            while (pos < str.length()) {
                size_t bytesRead;
                utf8ToCodePoint(std::string_view(str.data() + pos, str.length() - pos), bytesRead);
                if (bytesRead == 0) break;
                length++;
                pos += bytesRead;
            }
            return length;
        }

        bool Unicode::isValidUtf8(const std::string& str) {
            size_t pos = 0;
            while (pos < str.length()) {
                size_t bytesRead;
                uint32_t cp = utf8ToCodePoint(std::string_view(str.data() + pos, str.length() - pos), bytesRead);
                if (bytesRead == 0 || cp == 0) return false;
                pos += bytesRead;
            }
            return true;
        }

        uint32_t Unicode::utf8ToCodePoint(std::string_view sv, size_t& bytesRead) {
            bytesRead = 0;
            if (sv.empty()) return 0;

            uint8_t firstByte = static_cast<uint8_t>(sv[0]);
            uint32_t codePoint;

            if ((firstByte & UTF8_1BYTE_MASK) == 0) {
                // 1字节序列
                bytesRead = 1;
                return firstByte;
            } else if ((firstByte & UTF8_3BYTE_MASK) == UTF8_2BYTE_MASK) {
                // 2字节序列
                if (sv.length() < 2 || !isUtf8ContinuationByte(sv[1])) return 0;
                bytesRead = 2;
                codePoint = ((firstByte & 0x1F) << 6) | (static_cast<uint8_t>(sv[1]) & 0x3F);
            } else if ((firstByte & UTF8_4BYTE_MASK) == UTF8_3BYTE_MASK) {
                // 3字节序列
                if (sv.length() < 3 || !isUtf8ContinuationByte(sv[1]) || !isUtf8ContinuationByte(sv[2])) return 0;
                bytesRead = 3;
                codePoint = ((firstByte & 0x0F) << 12) | ((static_cast<uint8_t>(sv[1]) & 0x3F) << 6) |
                            (static_cast<uint8_t>(sv[2]) & 0x3F);
            } else if ((firstByte & UTF8_4BYTE_MASK) == UTF8_4BYTE_MASK) {
                // 4字节序列
                if (sv.length() < 4 || !isUtf8ContinuationByte(sv[1]) || !isUtf8ContinuationByte(sv[2]) ||
                    !isUtf8ContinuationByte(sv[3]))
                    return 0;
                bytesRead = 4;
                codePoint = ((firstByte & 0x07) << 18) | ((static_cast<uint8_t>(sv[1]) & 0x3F) << 12) |
                            ((static_cast<uint8_t>(sv[2]) & 0x3F) << 6) | (static_cast<uint8_t>(sv[3]) & 0x3F);
            } else {
                return 0;  // 无效的UTF-8序列
            }

            // 验证码点范围
            if (!isValidCodepoint(codePoint)) {
                bytesRead = 0;
                return 0;
            }

            return codePoint;
        }

        bool Unicode::isUtf8ContinuationByte(unsigned char byte) { return (byte & 0xC0) == 0x80; }

        bool Unicode::isValidCodepoint(uint32_t codepoint) {
            return codepoint <= 0x10FFFF && (codepoint < 0xD800 || codepoint > 0xDFFF);  // 排除代理对范围
        }

        std::string Unicode::codePointToUtf8(uint32_t codePoint) {
            if (!isValidCodepoint(codePoint)) {
                return std::string();  // 返回空字符串表示无效码点
            }

            std::string result;
            if (codePoint <= 0x7F) {
                // 1字节序列
                result.push_back(static_cast<char>(codePoint));
            } else if (codePoint <= 0x7FF) {
                // 2字节序列
                result.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
                result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            } else if (codePoint <= 0xFFFF) {
                // 3字节序列
                result.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
                result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            } else {
                // 4字节序列
                result.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
                result.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            }

            return result;
        }

    }  // namespace frontend
}  // namespace rp
