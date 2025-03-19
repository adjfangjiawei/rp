#include "UnicodeEscape.h"

#include <iomanip>
#include <sstream>

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {

    UnicodeEscape::EscapeResult UnicodeEscape::parseEscapeSequence(const std::string& input, size_t start) {
        if (start >= input.length()) {
            return {false, 0, 0, "Unexpected end of input"};
        }

        char c = input[start];

        // 处理基本转义序列
        switch (c) {
            case 'n':
                return {true, '\n', 1, ""};
            case 't':
                return {true, '\t', 1, ""};
            case 'r':
                return {true, '\r', 1, ""};
            case '\\':
                return {true, '\\', 1, ""};
            case '"':
                return {true, '"', 1, ""};
            case '\'':
                return {true, '\'', 1, ""};
            case 'x':  // 十六进制转义序列
                if (start + 2 >= input.length()) {
                    return {false, 0, 0, "Incomplete hex escape sequence"};
                }
                return parseHexEscape(input, start + 1, 2);
            case 'u':  // Unicode转义序列
                return parseUnicodeEscape(input, start + 1);
            case 'U':  // 扩展Unicode转义序列
                return parseExtendedUnicodeEscape(input, start + 1);
            default:
                // 检查八进制转义序列
                if (c >= '0' && c <= '7') {
                    return parseOctalEscape(input, start);
                }
                return {false, 0, 0, "Invalid escape sequence"};
        }
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseBasicEscape(char c) {
        switch (c) {
            case 'n':
                return {true, '\n', 1, ""};
            case 't':
                return {true, '\t', 1, ""};
            case 'r':
                return {true, '\r', 1, ""};
            case '\\':
                return {true, '\\', 1, ""};
            case '"':
                return {true, '"', 1, ""};
            case '\'':
                return {true, '\'', 1, ""};
            default:
                return {false, 0, 0, "Invalid basic escape sequence"};
        }
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseHexEscape(const std::string& input, size_t start, size_t length) {
        if (start + length > input.length()) {
            return {false, 0, 0, "Incomplete hex escape sequence"};
        }

        uint32_t value = 0;
        for (size_t i = 0; i < length; ++i) {
            char c = input[start + i];
            if (!isHexDigit(c)) {
                return {false, 0, i, "Invalid hex digit"};
            }
            value = (value << 4) | hexDigitToValue(c);
        }

        return {true, value, length + 1, ""};  // +1 for 'x'
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseOctalEscape(const std::string& input, size_t start) {
        if (start >= input.length()) {
            return {false, 0, 0, "Incomplete octal escape sequence"};
        }

        uint32_t value = octalDigitToValue(input[start]);
        size_t consumed = 1;

        // 最多读取3位八进制数
        for (size_t i = 1; i < 3 && (start + i) < input.length(); ++i) {
            char c = input[start + i];
            if (!isOctalDigit(c)) {
                break;
            }
            value = (value << 3) | octalDigitToValue(c);
            consumed++;
        }

        if (value > 0xFF) {
            return {false, 0, consumed, "Octal escape sequence too large"};
        }

        return {true, value, consumed, ""};
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseUnicodeEscape(const std::string& input, size_t start) {
        auto result = parseHexEscape(input, start, 4);
        if (result.success) {
            result.consumed += 1;  // 加上'u'的长度
            if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
                return {false, 0, result.consumed, "Invalid Unicode codepoint"};
            }
        }
        return result;
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseExtendedUnicodeEscape(const std::string& input, size_t start) {
        auto result = parseHexEscape(input, start, 8);
        if (result.success) {
            result.consumed += 1;  // 加上'U'的长度
            if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
                return {false, 0, result.consumed, "Invalid Unicode codepoint"};
            }
        }
        return result;
    }

    std::string UnicodeEscape::generateEscapeSequence(uint32_t codepoint) {
        std::stringstream ss;
        if (codepoint <= 0xFFFF) {
            ss << "\\u" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << codepoint;
        } else {
            ss << "\\U" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << codepoint;
        }
        return ss.str();
    }

    bool UnicodeEscape::validateEscapeSequence(const std::string& input, std::string& error) {
        if (input.empty()) {
            error = "Empty escape sequence";
            return false;
        }

        // 检查转义序列的开始
        if (input[0] != '\\') {
            error = "Escape sequence must start with '\\'";
            return false;
        }

        // 如果只有一个反斜杠
        if (input.length() == 1) {
            error = "Incomplete escape sequence";
            return false;
        }

        // 解析转义序列
        auto result = parseEscapeSequence(input, 1);
        if (!result.success) {
            error = result.error;
            return false;
        }

        return true;
    }

    UnicodeEscape::EscapeResult UnicodeEscape::processLineContinuation(const std::string& input) {
        if (input.empty()) {
            return {false, 0, 0, "Empty input"};
        }

        size_t pos = 0;
        size_t len = input.length();

        // 查找反斜杠
        while (pos < len) {
            if (input[pos] == '\\') {
                // 检查是否是行末
                if (pos + 1 >= len) {
                    return {true, 0, pos + 1, ""};
                }

                // 检查下一个字符
                char next = input[pos + 1];
                if (next == '\n') {
                    return {true, 0, pos + 2, ""};
                } else if (next == '\r') {
                    // 检查是否是\r\n
                    if (pos + 2 < len && input[pos + 2] == '\n') {
                        return {true, 0, pos + 3, ""};
                    }
                    return {true, 0, pos + 2, ""};
                }
            }
            ++pos;
        }

        return {false, 0, 0, "No line continuation found"};
    }

    bool UnicodeEscape::isHexDigit(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    uint32_t UnicodeEscape::hexDigitToValue(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return 0;
    }

    bool UnicodeEscape::isOctalDigit(char c) { return c >= '0' && c <= '7'; }

    uint32_t UnicodeEscape::octalDigitToValue(char c) { return c - '0'; }

}  // namespace rp::frontend::unicode
