#include "StringLiteralUtils.h"

#include "Frontend/Parser/Lexer/Token/TokenKind.h"
#include "Frontend/Parser/Lexer/Unicode/Core/UnicodeCore.h"
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"

namespace rp {
    namespace frontend {

        TokenKind StringLiteralUtils::getPrefixTokenKind(StringPrefix prefix) {
            switch (prefix) {
                case StringPrefix::None:
                    return TokenKind::StringLiteral;
                case StringPrefix::L:
                    return TokenKind::WideStringLiteral;
                case StringPrefix::u:
                    return TokenKind::UTF16StringLiteral;
                case StringPrefix::U:
                    return TokenKind::UTF32StringLiteral;
                case StringPrefix::u8:
                    return TokenKind::UTF8StringLiteral;
                case StringPrefix::R:
                    return TokenKind::RawStringLiteral;
                case StringPrefix::LR:
                    return TokenKind::WideStringLiteral;
                case StringPrefix::uR:
                    return TokenKind::UTF16StringLiteral;
                case StringPrefix::UR:
                    return TokenKind::UTF32StringLiteral;
                case StringPrefix::u8R:
                    return TokenKind::UTF8StringLiteral;
                default:
                    return TokenKind::Invalid;
            }
        }

        bool StringLiteralUtils::isWhitespace(char c) { return c == ' ' || c == '\t'; }

        std::string StringLiteralUtils::getErrorMessage(StringError error, size_t pos, const std::string& context) {
            switch (error) {
                case StringError::InvalidUTF8Sequence:
                    return "Invalid UTF-8 sequence at position " + std::to_string(pos);
                case StringError::UnterminatedString:
                    return "Unterminated string literal";
                case StringError::InvalidEscapeSequence:
                    return "Invalid escape sequence at position " + std::to_string(pos);
                case StringError::InvalidDelimiter:
                    return "Invalid raw string delimiter: " + context;
                case StringError::InvalidPrefix:
                    return "Invalid string prefix: " + context;
                default:
                    return "Unknown error";
            }
        }

        bool StringLiteralUtils::isValidRawStringDelimiter(const std::string& delimiter) {
            if (delimiter.empty()) return true;
            for (char c : delimiter) {
                if (!std::isalnum(c) && c != '_') return false;
            }
            return true;
        }

        bool StringLiteralUtils::isOctalDigit(char c) { return c >= '0' && c <= '7'; }

        bool StringLiteralUtils::isHexDigit(char c) {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        }

        int StringLiteralUtils::hexDigitToInt(char c) {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }

        std::string StringLiteralUtils::unicodeToUTF8(unsigned int codepoint) {
            return unicode::UnicodeEncoding::encodeUtf8(codepoint);
        }

        bool StringLiteralUtils::isValidUTF8StartByte(unsigned char c) {
            return unicode::UnicodeCore::isValidUtf8FirstByte(c);
        }

        size_t StringLiteralUtils::getUTF8ByteCount(unsigned char c) {
            return unicode::UnicodeCore::getUtf8ByteCount(c);
        }

        bool StringLiteralUtils::isValidUTF8ContinuationByte(unsigned char c) {
            return unicode::UnicodeCore::isUtf8ContinuationByte(c);
        }

        std::tuple<uint32_t, size_t> StringLiteralUtils::getUTF8Char(const std::string& str, size_t pos) {
            return unicode::UnicodeEncoding::getMultiByteChar(str, pos);
        }

        std::tuple<bool, size_t> StringLiteralUtils::validateUTF8Sequence(const std::string& str, size_t pos) {
            if (pos >= str.length()) {
                return {false, 0};
            }

            unsigned char firstByte = static_cast<unsigned char>(str[pos]);

            // ASCII字符
            if (firstByte < 0x80) {
                return {true, 1};
            }

            // 获取UTF-8序列长度
            size_t expectedLength = unicode::UnicodeCore::getUtf8SequenceLength(firstByte);
            if (expectedLength == 0 || pos + expectedLength > str.length()) {
                return {false, 0};
            }

            // 验证后续字节
            for (size_t i = 1; i < expectedLength; i++) {
                if (!unicode::UnicodeCore::isUtf8ContinuationByte(static_cast<unsigned char>(str[pos + i]))) {
                    return {false, 0};
                }
            }

            // 验证完整的UTF-8序列
            auto [codepoint, actualLength] = unicode::UnicodeEncoding::getMultiByteChar(str, pos);
            if (actualLength != expectedLength) {
                return {false, 0};
            }

            // 验证码点的有效范围
            if (!unicode::UnicodeCore::isValidCodepoint(codepoint)) {
                return {false, 0};
            }

            // 验证最小编码长度
            size_t minimumLength = unicode::UnicodeCore::getUtf8ByteCount(codepoint);
            if (expectedLength < minimumLength) {
                return {false, 0};
            }

            return {true, actualLength};
        }

        bool StringLiteralUtils::validateCompleteUTF8String(const std::string& str, std::string& errorMsg) {
            size_t pos = 0;
            while (pos < str.length()) {
                auto [isValid, length] = validateUTF8Sequence(str, pos);
                if (!isValid) {
                    errorMsg = "Invalid UTF-8 sequence at position " + std::to_string(pos);
                    return false;
                }
                // 防止死循环：如果length为0，至少前进一个字节
                if (length == 0) {
                    pos++;
                } else {
                    pos += length;
                }
            }
            return true;
        }

        std::tuple<StringPrefix, size_t> StringLiteralUtils::parseStringPrefix(const std::string& input) {
            if (input.empty()) return {StringPrefix::None, 0};

            // 检查两字符前缀
            if (input.length() >= 3) {
                std::string prefix = input.substr(0, 2);
                if (prefix == "u8" && input[2] == 'R') return {StringPrefix::u8R, 3};
                if (prefix == "u8") return {StringPrefix::u8, 2};
                if (prefix == "LR") return {StringPrefix::LR, 2};
                if (prefix == "uR") return {StringPrefix::uR, 2};
                if (prefix == "UR") return {StringPrefix::UR, 2};
            }

            // 检查单字符前缀
            if (input.length() >= 2) {
                char prefix = input[0];
                bool isRaw = (input[1] == 'R');
                switch (prefix) {
                    case 'L':
                        return isRaw ? std::make_tuple(StringPrefix::LR, 2) : std::make_tuple(StringPrefix::L, 1);
                    case 'u':
                        return isRaw ? std::make_tuple(StringPrefix::uR, 2) : std::make_tuple(StringPrefix::u, 1);
                    case 'U':
                        return isRaw ? std::make_tuple(StringPrefix::UR, 2) : std::make_tuple(StringPrefix::U, 1);
                    case 'R':
                        return {StringPrefix::R, 1};
                    default:
                        return {StringPrefix::None, 0};
                }
            }

            // 检查单个字符
            char prefix = input[0];
            switch (prefix) {
                case 'L':
                    return {StringPrefix::L, 1};
                case 'u':
                    return {StringPrefix::u, 1};
                case 'U':
                    return {StringPrefix::U, 1};
                case 'R':
                    return {StringPrefix::R, 1};
                default:
                    return {StringPrefix::None, 0};
            }
        }

        bool StringLiteralUtils::isValidStringPrefix(const std::string& prefix) {
            if (prefix.empty()) return true;
            auto [kind, length] = parseStringPrefix(prefix);
            return kind != StringPrefix::None && length == prefix.length();
        }

        bool StringLiteralUtils::isRawStringPrefix(StringPrefix prefix) {
            return prefix == StringPrefix::R || prefix == StringPrefix::LR || prefix == StringPrefix::uR ||
                   prefix == StringPrefix::UR || prefix == StringPrefix::u8R;
        }

        std::string StringLiteralUtils::getPrefixString(StringPrefix prefix) {
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

        bool StringLiteralUtils::isValidStringChar(unsigned char c) {
            // 控制字符检查
            if (c < 0x20) {
                return c == '\t' || c == '\n' || c == '\r';
            }
            // 其他ASCII字符都是有效的
            if (c < 0x80) return true;
            // 对于非ASCII字符，需要作为UTF-8序列的一部分进行验证
            return isValidUTF8StartByte(c) || isValidUTF8ContinuationByte(c);
        }

        bool StringLiteralUtils::hasValidQuotes(const std::string& str) {
            if (str.length() < 2) return false;

            // 检查开始引号
            size_t startPos = 0;
            size_t endPos = str.length() - 1;

            // 处理ASCII引号
            if (str[startPos] == '"' && str[endPos] == '"') return true;
            if (str[startPos] == '\'' && str[endPos] == '\'') return true;

            // 处理UTF-8中文引号
            if (str.length() >= 6) {  // 最少需要6个字节（3字节引号 + 3字节引号）
                auto [startCodepoint, startLength] = getUTF8Char(str, startPos);
                if (startLength > 0) {
                    auto [endCodepoint, endLength] = getUTF8Char(str, str.length() - 3);  // 假设UTF-8引号是3字节
                    if (endLength > 0) {
                        // 检查中文引号对
                        if ((startCodepoint == 0x300C && endCodepoint == 0x300D) ||  // 「」
                            (startCodepoint == 0x300E && endCodepoint == 0x300F) ||  // 『』
                            (startCodepoint == 0x2018 && endCodepoint == 0x2019) ||  // ''
                            (startCodepoint == 0x201C && endCodepoint == 0x201D))    // ""
                            return true;
                    }
                }
            }

            return false;
        }

        bool StringLiteralUtils::isEscaped(const std::string& str, size_t pos) {
            if (pos == 0) return false;
            size_t backslashCount = 0;
            size_t i = pos - 1;
            while (i != size_t(-1) && str[i] == '\\') {
                backslashCount++;
                if (i == 0) break;
                i--;
            }
            return backslashCount % 2 == 1;
        }

        bool StringLiteralUtils::isUnescapedQuote(const std::string& str, size_t pos) {
            if (pos >= str.length()) return false;

            // 检查ASCII引号
            if ((str[pos] == '"' || str[pos] == '\'') && !isEscaped(str, pos)) return true;

            // 检查UTF-8中文引号
            if (static_cast<unsigned char>(str[pos]) >= 0x80) {
                auto [codepoint, length] = getUTF8Char(str, pos);
                if (length > 0) {
                    // 检查是否是中文引号或智能引号
                    return codepoint == 0x300C ||  // 「
                           codepoint == 0x300D ||  // 」
                           codepoint == 0x300E ||  // 『
                           codepoint == 0x300F ||  // 』
                           codepoint == 0x2018 ||  // '
                           codepoint == 0x2019 ||  // '
                           codepoint == 0x201C ||  // "
                           codepoint == 0x201D;    // "
                }
            }

            return false;
        }

    }  // namespace frontend
}  // namespace rp
