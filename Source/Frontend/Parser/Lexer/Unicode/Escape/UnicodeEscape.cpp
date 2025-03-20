#include "UnicodeEscape.h"

#include <iomanip>
#include <sstream>

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {

    namespace {
        // 常量定义
        constexpr char ESCAPE_CHAR = '\\';
        constexpr size_t HEX_ESCAPE_LENGTH = 2;
        constexpr size_t UNICODE_ESCAPE_LENGTH = 4;
        constexpr size_t EXTENDED_UNICODE_ESCAPE_LENGTH = 8;

        // 辅助函数
        constexpr bool isNewLine(char c) { return c == '\n' || c == '\r'; }
    }  // namespace

    UnicodeEscape::EscapeResult UnicodeEscape::parseEscapeSequence(const std::string& input, size_t start) {
        if (start >= input.length()) {
            return {false, 0, 0, "Unexpected end of input"};
        }

        char c = input[start];

        // 处理基本转义序列
        switch (c) {
            case 'a':
                return {true, '\a', 1, ""};  // 响铃
            case 'b':
                return {true, '\b', 1, ""};  // 退格
            case 'f':
                return {true, '\f', 1, ""};  // 换页
            case 'n':
                return {true, '\n', 1, ""};  // 换行
            case 'r':
                return {true, '\r', 1, ""};  // 回车
            case 't':
                return {true, '\t', 1, ""};  // 水平制表符
            case 'v':
                return {true, '\v', 1, ""};  // 垂直制表符
            case '\\':
                return {true, '\\', 1, ""};  // 反斜杠
            case '"':
                return {true, '"', 1, ""};  // 双引号
            case '\'':
                return {true, '\'', 1, ""};  // 单引号
            case '?':
                return {true, '?', 1, ""};  // 问号
            case 'x':                       // 十六进制转义序列
                if (start + 1 >= input.length()) {
                    return {false, 0, 0, "Incomplete hex escape sequence"};
                }
                return parseHexEscape(input, start + 1, HEX_ESCAPE_LENGTH);
            case 'u':  // Unicode转义序列
                if (start + 1 >= input.length()) {
                    return {false, 0, 0, "Incomplete Unicode escape sequence"};
                }
                return parseUnicodeEscape(input, start + 1);
            case 'U':  // 扩展Unicode转义序列
                if (start + 1 >= input.length()) {
                    return {false, 0, 0, "Incomplete extended Unicode escape sequence"};
                }
                return parseExtendedUnicodeEscape(input, start + 1);
            default:
                // 检查八进制转义序列
                if (isOctalDigit(c)) {
                    return parseOctalEscape(input, start);
                }
                return {false, 0, 0, "Invalid escape sequence character: " + std::string(1, c)};
        }
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseHexEscape(const std::string& input, size_t start, size_t length) {
        if (start + length > input.length()) {
            return {false, 0, 0, "Incomplete hex escape sequence: expected " + std::to_string(length) + " digits"};
        }

        uint32_t value = 0;
        for (size_t i = 0; i < length; ++i) {
            char c = input[start + i];
            if (!isHexDigit(c)) {
                return {false, 0, i, "Invalid hex digit: " + std::string(1, c)};
            }
            value = (value << 4) | hexDigitToValue(c);
        }

        return {true, value, length + 1, ""};  // +1 for 'x'/'u'/'U'
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
            uint32_t newValue = (value << 3) | octalDigitToValue(c);
            if (newValue > 0xFF) {
                break;  // 超出范围，停止读取
            }
            value = newValue;
            consumed++;
        }

        return {true, value, consumed, ""};
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseUnicodeEscape(const std::string& input, size_t start) {
        auto result = parseHexEscape(input, start, UNICODE_ESCAPE_LENGTH);
        if (result.success) {
            result.consumed += 1;  // 加上'u'的长度
            if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
                return {false, 0, result.consumed, "Invalid Unicode codepoint: U+" + std::to_string(result.codepoint)};
            }
        }
        return result;
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseExtendedUnicodeEscape(const std::string& input, size_t start) {
        auto result = parseHexEscape(input, start, EXTENDED_UNICODE_ESCAPE_LENGTH);
        if (result.success) {
            result.consumed += 1;  // 加上'U'的长度
            if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
                return {false, 0, result.consumed, "Invalid Unicode codepoint: U+" + std::to_string(result.codepoint)};
            }
        }
        return result;
    }

    std::string UnicodeEscape::generateEscapeSequence(uint32_t codepoint) {
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return "";  // 返回空字符串表示无效码点
        }

        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');

        // 根据码点值选择合适的转义序列格式
        if (codepoint < 0x20 || codepoint == 0x7F) {
            // 控制字符使用\x格式
            ss << "\\x" << std::setw(2) << codepoint;
        } else if (codepoint <= 0xFF) {
            // ASCII可打印字符和扩展ASCII使用\x格式
            ss << "\\x" << std::setw(2) << codepoint;
        } else if (codepoint <= 0xFFFF) {
            // 基本多语言平面使用\u格式
            ss << "\\u" << std::setw(4) << codepoint;
        } else {
            // 其他平面使用\U格式
            ss << "\\U" << std::setw(8) << codepoint;
        }

        return ss.str();
    }

    bool UnicodeEscape::validateEscapeSequence(const std::string& input, std::string& error) {
        if (input.empty()) {
            error = "Empty escape sequence";
            return false;
        }

        // 检查转义序列的开始
        if (input[0] != ESCAPE_CHAR) {
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

        // 检查是否有多余的字符
        if (result.consumed + 1 < input.length()) {
            error = "Extra characters after escape sequence";
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

        // 查找反斜杠和换行符组合
        while (pos < len) {
            if (input[pos] == ESCAPE_CHAR) {
                if (pos + 1 >= len) {
                    // 反斜杠在末尾
                    return {true, 0, pos + 1, ""};
                }

                char next = input[pos + 1];
                if (next == '\n') {
                    // 找到 \n
                    return {true, 0, pos + 2, ""};
                } else if (next == '\r') {
                    // 检查是否是 \r\n
                    if (pos + 2 < len && input[pos + 2] == '\n') {
                        return {true, 0, pos + 3, ""};
                    }
                    return {true, 0, pos + 2, ""};
                } else if (isNewLine(next)) {
                    // 其他换行符
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
