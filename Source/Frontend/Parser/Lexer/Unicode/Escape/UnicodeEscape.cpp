#include "UnicodeEscape.h"

#include <iomanip>
#include <sstream>

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {

    UnicodeEscape::EscapeResult UnicodeEscape::parseEscapeSequence(const std::string& input, size_t start) {
        if (start >= input.length()) {
            return {false, 0, 0, "Unexpected end of input"};
        }

        if (input[start] == 'u') {
            return parseUnicodeEscape(input, start + 1);
        } else if (input[start] == 'U') {
            return parseExtendedUnicodeEscape(input, start + 1);
        }

        return {false, 0, 0, "Invalid escape sequence"};
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseHexEscape(const std::string& input, size_t start, size_t length) {
        if (start + length > input.length()) {
            return {false, 0, 0, "Incomplete escape sequence"};
        }

        uint32_t value = 0;
        for (size_t i = 0; i < length; ++i) {
            char c = input[start + i];
            if (!isHexDigit(c)) {
                return {false, 0, i, "Invalid hex digit"};
            }
            value = (value << 4) | hexDigitToValue(c);
        }

        if (!UnicodeCore::isValidCodepoint(value)) {
            return {false, 0, length, "Invalid Unicode codepoint"};
        }

        return {true, value, length, ""};
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseUnicodeEscape(const std::string& input, size_t start) {
        auto result = parseHexEscape(input, start, 4);
        if (result.success) {
            result.consumed += 1;  // 加上'u'的长度
        }
        return result;
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseExtendedUnicodeEscape(const std::string& input, size_t start) {
        auto result = parseHexEscape(input, start, 8);
        if (result.success) {
            result.consumed += 1;  // 加上'U'的长度
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

        // 检查Unicode转义序列
        if (input[1] == 'u') {
            if (input.length() < 6) {  // \u + 4个十六进制数字
                error = "Incomplete Unicode escape sequence";
                return false;
            }
            auto result = parseUnicodeEscape(input, 1);
            if (!result.success) {
                error = result.error;
                return false;
            }
            return true;
        } else if (input[1] == 'U') {
            if (input.length() < 10) {  // \U + 8个十六进制数字
                error = "Incomplete extended Unicode escape sequence";
                return false;
            }
            auto result = parseExtendedUnicodeEscape(input, 1);
            if (!result.success) {
                error = result.error;
                return false;
            }
            return true;
        }

        error = "Invalid escape sequence type";
        return false;
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

}  // namespace rp::frontend::unicode
