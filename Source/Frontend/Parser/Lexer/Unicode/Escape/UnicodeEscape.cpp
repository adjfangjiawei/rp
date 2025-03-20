#include "UnicodeEscape.h"

#include <iomanip>
#include <sstream>
#include <unordered_map>

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {
    namespace {
        // 常量定义
        constexpr char ESCAPE_CHAR = '\\';
        constexpr size_t HEX_ESCAPE_LENGTH = 2;
        constexpr size_t UNICODE_ESCAPE_LENGTH = 4;
        constexpr size_t EXTENDED_UNICODE_ESCAPE_LENGTH = 8;

        // 使用查找表优化字符检查
        constexpr bool ASCII_TABLE[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,  // 0x00-0x0F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F (0-9)
            0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x40-0x4F (A-F)
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x50-0x5F
            0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x60-0x6F (a-f)
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // 0x70-0x7F
        };

        constexpr uint8_t HEX_VALUES[128] = {
            0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00-0x0F
            0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10-0x1F
            0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20-0x2F
            0, 1,  2,  3,  4,  5,  6,  7, 8, 9, 0, 0, 0, 0, 0, 0,  // 0x30-0x3F (0-9)
            0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x40-0x4F (A-F)
            0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x50-0x5F
            0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x60-0x6F (a-f)
            0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0   // 0x70-0x7F
        };

        // 辅助函数
        constexpr bool isNewLine(char c) { return c == '\n' || c == '\r'; }

        // 基本转义字符映射表
        const std::unordered_map<char, uint32_t> BASIC_ESCAPES = {
            {'a', '\a'},   // 响铃
            {'b', '\b'},   // 退格
            {'f', '\f'},   // 换页
            {'n', '\n'},   // 换行
            {'r', '\r'},   // 回车
            {'t', '\t'},   // 水平制表符
            {'v', '\v'},   // 垂直制表符
            {'\\', '\\'},  // 反斜杠
            {'\"', '\"'},  // 双引号
            {'\'', '\''},  // 单引号
            {'?', '\?'},   // 问号
        };
    }  // namespace

    bool UnicodeEscape::isHexDigit(char c) { return c >= 0 && c < 128 && ASCII_TABLE[c]; }

    uint32_t UnicodeEscape::hexDigitToValue(char c) { return (c >= 0 && c < 128) ? HEX_VALUES[c] : 0; }

    bool UnicodeEscape::isOctalDigit(char c) { return c >= '0' && c <= '7'; }

    uint32_t UnicodeEscape::octalDigitToValue(char c) { return (c >= '0' && c <= '7') ? (c - '0') : 0; }

    UnicodeEscape::EscapeResult UnicodeEscape::parseEscapeSequence(const std::string& input, size_t start) {
        // 边界检查
        if (start >= input.length()) {
            return {false, 0, 0, "Unexpected end of input"};
        }

        char c = input[start];

        // 处理基本转义序列
        auto it = BASIC_ESCAPES.find(c);
        if (it != BASIC_ESCAPES.end()) {
            return {true, it->second, 1, ""};
        }

        // 处理特殊转义序列
        switch (c) {
            case 'x':
                {  // 十六进制转义序列
                    if (start + 1 >= input.length()) {
                        return {false, 0, 0, "Incomplete hex escape sequence"};
                    }
                    auto result = parseHexEscape(input, start + 1, HEX_ESCAPE_LENGTH);
                    if (result.success) {
                        // 验证十六进制值是否在有效范围内
                        if (result.codepoint > 0xFF) {
                            return {false,
                                    0,
                                    result.consumed,
                                    "Hex escape sequence value out of range: 0x" + std::to_string(result.codepoint)};
                        }
                    }
                    return result;
                }

            case 'u':
                {  // Unicode转义序列
                    if (start + 1 >= input.length()) {
                        return {false, 0, 0, "Incomplete Unicode escape sequence"};
                    }
                    auto result = parseUnicodeEscape(input, start + 1);
                    if (result.success) {
                        // 验证Unicode码点的有效性
                        if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
                            return {false,
                                    0,
                                    result.consumed,
                                    "Invalid Unicode codepoint: U+" + std::to_string(result.codepoint)};
                        }
                    }
                    return result;
                }

            case 'U':
                {  // 扩展Unicode转义序列
                    if (start + 1 >= input.length()) {
                        return {false, 0, 0, "Incomplete extended Unicode escape sequence"};
                    }
                    auto result = parseExtendedUnicodeEscape(input, start + 1);
                    if (result.success) {
                        // 验证扩展Unicode码点的有效性
                        if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
                            return {false,
                                    0,
                                    result.consumed,
                                    "Invalid extended Unicode codepoint: U+" + std::to_string(result.codepoint)};
                        }
                        // 检查是否超出Unicode范围
                        if (result.codepoint > 0x10FFFF) {
                            return {
                                false,
                                0,
                                result.consumed,
                                "Extended Unicode escape sequence out of range: U+" + std::to_string(result.codepoint)};
                        }
                    }
                    return result;
                }

            default:
                // 检查八进制转义序列
                if (isOctalDigit(c)) {
                    auto result = parseOctalEscape(input, start);
                    if (result.success) {
                        // 验证八进制值是否在有效范围内
                        if (result.codepoint > 0xFF) {
                            return {false,
                                    0,
                                    result.consumed,
                                    "Octal escape sequence value out of range: " + std::to_string(result.codepoint)};
                        }
                    }
                    return result;
                }

                // 无效的转义序列
                return {false, 0, 0, "Invalid escape sequence character: '" + std::string(1, c) + "'"};
        }
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseHexEscape(const std::string& input, size_t start, size_t length) {
        // 验证输入长度
        if (start + length > input.length()) {
            return {false,
                    0,
                    0,
                    "Incomplete hex escape sequence: expected " + std::to_string(length) + " digits, found " +
                        std::to_string(input.length() - start)};
        }

        uint32_t value = 0;
        size_t i;

        // 使用查找表快速解析十六进制数字
        for (i = 0; i < length; ++i) {
            char c = input[start + i];

            // 验证字符是否为有效的十六进制数字
            if (!isHexDigit(c)) {
                std::stringstream ss;
                ss << "Invalid hex digit '" << c << "' at position " << (start + i) << " (expected 0-9, A-F, or a-f)";
                return {false, 0, i, ss.str()};
            }

            // 使用查找表获取数字值
            value = (value << 4) | hexDigitToValue(c);
        }

        // 返回解析结果
        return {true, value, length, ""};
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseOctalEscape(const std::string& input, size_t start) {
        // 验证输入长度
        if (start >= input.length()) {
            return {false, 0, 0, "Incomplete octal escape sequence"};
        }

        // 检查第一个字符是否为有效的八进制数字
        if (!isOctalDigit(input[start])) {
            return {false, 0, 0, "Invalid octal digit '" + std::string(1, input[start]) + "' (expected 0-7)"};
        }

        uint32_t value = octalDigitToValue(input[start]);
        size_t consumed = 1;

        // 最多读取3位八进制数，但要确保不超过0xFF (255)
        for (size_t i = 1; i < 3 && (start + i) < input.length(); ++i) {
            char c = input[start + i];

            // 检查是否为有效的八进制数字
            if (!isOctalDigit(c)) {
                break;
            }

            // 计算新值
            uint32_t newValue = (value << 3) | octalDigitToValue(c);

            // 检查是否超出范围
            if (newValue > 0xFF) {
                std::stringstream ss;
                ss << "Octal escape sequence value " << std::oct << newValue
                   << " exceeds maximum allowed value (377 octal = 255 decimal)";
                return {false, 0, consumed, ss.str()};
            }

            value = newValue;
            consumed++;
        }

        // 返回解析结果
        return {true, value, consumed, ""};
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseUnicodeEscape(const std::string& input, size_t start) {
        // 解析4位十六进制数字
        auto result = parseHexEscape(input, start, UNICODE_ESCAPE_LENGTH);
        if (!result.success) {
            // 添加更多上下文信息到错误消息
            result.error = "Invalid Unicode escape sequence (\\u): " + result.error;
            return result;
        }

        // 验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
            std::stringstream ss;
            ss << "Invalid Unicode codepoint U+" << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
               << result.codepoint;
            if (result.codepoint >= 0xD800 && result.codepoint <= 0xDFFF) {
                ss << " (surrogate pair code points are not allowed)";
            } else {
                ss << " (value out of valid Unicode range)";
            }
            return {false, 0, result.consumed, ss.str()};
        }

        return {true, result.codepoint, result.consumed, ""};
    }

    UnicodeEscape::EscapeResult UnicodeEscape::parseExtendedUnicodeEscape(const std::string& input, size_t start) {
        // 解析8位十六进制数字
        auto result = parseHexEscape(input, start, EXTENDED_UNICODE_ESCAPE_LENGTH);
        if (!result.success) {
            // 添加更多上下文信息到错误消息
            result.error = "Invalid extended Unicode escape sequence (\\U): " + result.error;
            return result;
        }

        // 验证码点范围
        if (result.codepoint > 0x10FFFF) {
            std::stringstream ss;
            ss << "Extended Unicode escape sequence U+" << std::hex << std::uppercase << std::setfill('0')
               << std::setw(8) << result.codepoint << " exceeds maximum allowed value (U+10FFFF)";
            return {false, 0, result.consumed, ss.str()};
        }

        // 验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(result.codepoint)) {
            std::stringstream ss;
            ss << "Invalid extended Unicode codepoint U+" << std::hex << std::uppercase << std::setfill('0')
               << std::setw(8) << result.codepoint;
            if (result.codepoint >= 0xD800 && result.codepoint <= 0xDFFF) {
                ss << " (surrogate pair code points are not allowed)";
            } else {
                ss << " (value out of valid Unicode range)";
            }
            return {false, 0, result.consumed, ss.str()};
        }

        return {true, result.codepoint, result.consumed, ""};
    }

    std::string UnicodeEscape::generateEscapeSequence(uint32_t codepoint) {
        // 验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return "";  // 返回空字符串表示无效码点
        }

        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');

        // 根据码点值选择最优的转义序列格式
        if (codepoint < 0x20) {
            // 控制字符使用标准转义序列（如果可用）
            switch (codepoint) {
                case '\a':
                    return "\\a";  // 响铃
                case '\b':
                    return "\\b";  // 退格
                case '\f':
                    return "\\f";  // 换页
                case '\n':
                    return "\\n";  // 换行
                case '\r':
                    return "\\r";  // 回车
                case '\t':
                    return "\\t";  // 水平制表符
                case '\v':
                    return "\\v";  // 垂直制表符
                default:
                    ss << "\\x" << std::setw(2) << codepoint;
            }
        } else if (codepoint < 0x7F) {
            // ASCII可打印字符
            switch (codepoint) {
                case '\\':
                    return "\\\\";  // 反斜杠
                case '\"':
                    return "\\\"";  // 双引号
                case '\'':
                    return "\\\'";  // 单引号
                case '\?':
                    return "\\?";  // 问号
                default:
                    // 其他可打印ASCII字符直接使用
                    return std::string(1, static_cast<char>(codepoint));
            }
        } else if (codepoint <= 0xFF) {
            // 扩展ASCII使用\x格式
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
        // 基本验证
        if (input.empty()) {
            error = "Empty escape sequence";
            return false;
        }

        // 检查转义序列的开始
        if (input[0] != ESCAPE_CHAR) {
            error = "Escape sequence must start with '\\', found '" + std::string(1, input[0]) + "'";
            return false;
        }

        // 检查序列长度
        if (input.length() == 1) {
            error = "Incomplete escape sequence: isolated backslash";
            return false;
        }

        // 预检查常见的无效字符
        char secondChar = input[1];
        if (!isprint(secondChar)) {
            std::stringstream ss;
            ss << "Invalid escape sequence character: "
               << "non-printable character (ASCII " << static_cast<int>(secondChar) << ")";
            error = ss.str();
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
            std::stringstream ss;
            ss << "Extra characters after escape sequence: '" << input.substr(result.consumed + 1)
               << "' (escape sequence ends at position " << result.consumed << ")";
            error = ss.str();
            return false;
        }

        return true;
    }

    UnicodeEscape::EscapeResult UnicodeEscape::processLineContinuation(const std::string& input) {
        // 基本验证
        if (input.empty()) {
            return {false, 0, 0, "Empty input"};
        }

        size_t pos = 0;
        size_t len = input.length();

        // 查找反斜杠和换行符组合
        while (pos < len) {
            if (input[pos] == ESCAPE_CHAR) {
                // 检查是否到达字符串末尾
                if (pos + 1 >= len) {
                    return {true, 0, pos + 1, "Warning: Backslash at end of input"};
                }

                char next = input[pos + 1];

                // 处理不同类型的换行符
                if (next == '\n') {
                    // Unix风格换行符 (\n)
                    return {true, 0, pos + 2, ""};
                } else if (next == '\r') {
                    // 检查Windows风格换行符 (\r\n)
                    if (pos + 2 < len && input[pos + 2] == '\n') {
                        return {true, 0, pos + 3, ""};
                    }
                    // Mac风格换行符 (\r)
                    return {true, 0, pos + 2, ""};
                } else if (isspace(next)) {
                    // 其他空白字符
                    std::stringstream ss;
                    ss << "Warning: Backslash followed by unexpected whitespace "
                       << "character (ASCII " << static_cast<int>(next) << ")";
                    return {true, 0, pos + 2, ss.str()};
                }
            }
            ++pos;
        }

        return {false, 0, 0, "No line continuation found"};
    }

}  // namespace rp::frontend::unicode
