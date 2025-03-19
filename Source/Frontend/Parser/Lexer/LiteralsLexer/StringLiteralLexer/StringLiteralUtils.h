
#ifndef STRINGLITERALUTILS_H
#define STRINGLITERALUTILS_H

#include <string>
#include <tuple>

namespace rp {
    namespace frontend {

        // 字符串字面量前缀类型
        enum class StringPrefix {
            None,  // 无前缀
            L,     // L"..."
            u,     // u"..."
            U,     // U"..."
            u8,    // u8"..."
            R,     // R"..."
            LR,    // LR"..."
            uR,    // uR"..."
            UR,    // UR"..."
            u8R    // u8R"..."
        };

        class StringLiteralUtils {
          public:
            // 现有的方法
            static bool isOctalDigit(char c);
            static bool isHexDigit(char c);
            static int hexDigitToInt(char c);
            static std::string unicodeToUTF8(unsigned int codepoint);

            // UTF-8 相关功能
            static bool isValidUTF8StartByte(unsigned char c);
            static size_t getUTF8ByteCount(unsigned char c);
            static bool isValidUTF8ContinuationByte(unsigned char c);
            static std::tuple<bool, size_t> validateUTF8Sequence(const std::string& str, size_t pos);

            // 字符串前缀相关功能
            static std::tuple<StringPrefix, size_t> parseStringPrefix(const std::string& input) {
                if (input.empty() || input[0] == '"') {
                    return {StringPrefix::None, 0};
                }

                // 检查最长的前缀
                if (input.length() >= 4 && input.compare(0, 3, "u8R") == 0 && input[3] == '"') {
                    return {StringPrefix::u8R, 3};
                }
                if (input.length() >= 3) {
                    if (input.compare(0, 2, "u8") == 0 && input[2] == '"') {
                        return {StringPrefix::u8, 2};
                    }
                    if (input[1] == 'R' && input[2] == '"') {
                        if (input[0] == 'L') return {StringPrefix::LR, 2};
                        if (input[0] == 'u') return {StringPrefix::uR, 2};
                        if (input[0] == 'U') return {StringPrefix::UR, 2};
                    }
                }
                if (input.length() >= 2) {
                    if (input[1] == '"') {
                        if (input[0] == 'L') return {StringPrefix::L, 1};
                        if (input[0] == 'u') return {StringPrefix::u, 1};
                        if (input[0] == 'U') return {StringPrefix::U, 1};
                        if (input[0] == 'R') return {StringPrefix::R, 1};
                    }
                }
                return {StringPrefix::None, 0};
            }

            static bool isValidStringPrefix(const std::string& prefix) {
                return prefix.empty() || prefix == "L" || prefix == "u" || prefix == "U" || prefix == "u8" ||
                       prefix == "R" || prefix == "LR" || prefix == "uR" || prefix == "UR" || prefix == "u8R";
            }

            static bool isRawStringPrefix(StringPrefix prefix) {
                return prefix == StringPrefix::R || prefix == StringPrefix::LR || prefix == StringPrefix::uR ||
                       prefix == StringPrefix::UR || prefix == StringPrefix::u8R;
            }

            // 字符串处理辅助方法
            static bool isWhitespace(char c) { return c == ' ' || c == '\t'; }

            static bool isValidStringChar(unsigned char c) {
                // ASCII范围的字符
                if (c < 0x80) {
                    return c >= 0x20 || c == '\t' || c == '\n' || c == '\r';
                }
                // 对于非ASCII字符，必须是有效的UTF-8起始字节
                return isValidUTF8StartByte(c);
            }

            static bool hasValidQuotes(const std::string& str) {
                return str.length() >= 2 && str.front() == '"' && str.back() == '"';
            }

            static bool isUnescapedQuote(const std::string& str, size_t pos) {
                if (pos >= str.length() || str[pos] != '"') return false;

                // 计算前面连续的反斜杠数量
                size_t backslashCount = 0;
                if (pos > 0) {
                    size_t i = pos - 1;
                    while (i < str.length() && i >= 0 && str[i] == '\\') {
                        backslashCount++;
                        if (i == 0) break;
                        i--;
                    }
                }
                // 如果反斜杠数量为偶数，则引号未被转义
                return backslashCount % 2 == 0;
            }

            // 获取前缀对应的字符串
            static std::string getPrefixString(StringPrefix prefix) {
                switch (prefix) {
                    case StringPrefix::None:
                        return "";
                    case StringPrefix::L:
                        return "L";
                    case StringPrefix::u:
                        return "u";
                    case StringPrefix::U:
                        return "U";
                    case StringPrefix::u8:
                        return "u8";
                    case StringPrefix::R:
                        return "R";
                    case StringPrefix::LR:
                        return "LR";
                    case StringPrefix::uR:
                        return "uR";
                    case StringPrefix::UR:
                        return "UR";
                    case StringPrefix::u8R:
                        return "u8R";
                    default:
                        return "";
                }
            }
        };

    }  // namespace frontend
}  // namespace rp

#endif  // STRINGLITERALUTILS_H
