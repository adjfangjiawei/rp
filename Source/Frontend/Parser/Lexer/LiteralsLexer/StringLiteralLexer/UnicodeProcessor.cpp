#include "UnicodeProcessor.h"

#include <sstream>
#include <stdexcept>

namespace rp {
    namespace frontend {

        std::string UnicodeProcessor::processUnicodeEscape(const std::string& source,
                                                           size_t& currentPos,
                                                           bool isLongForm,
                                                           std::string& error) {
            size_t length = isLongForm ? 8 : 4;
            if (currentPos + length >= source.length()) {
                error = "不完整的Unicode转义序列";
                return "";
            }

            try {
                std::string unicode = source.substr(currentPos, length);
                uint32_t codepoint = parseHexValue(unicode, error);
                if (!error.empty()) {
                    return "";
                }

                // 验证码点值
                if (!isValidCodePoint(codepoint)) {
                    if (codepoint > 0x10FFFF) {
                        error = "Unicode码点超出有效范围 (0x0-0x10FFFF): " + unicode;
                    } else {
                        error = "无效的Unicode码点值: " + unicode;
                    }
                    return "";
                }

                // 处理代理对范围
                if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
                    error = "不允许使用代理对范围 (0xD800-0xDFFF) 的码点: " + unicode;
                    return "";
                }

                currentPos += length;
                return codePointToUTF8(codepoint);
            } catch (const std::exception& e) {
                error = "处理Unicode转义序列时出错: " + std::string(e.what());
                return "";
            }
        }

        std::string UnicodeProcessor::codePointToUTF8(uint32_t codepoint) {
            std::string result;
            result.reserve(4);  // UTF-8最多需要4个字节

            try {
                if (codepoint <= 0x7F) {
                    // 1字节序列: 0xxxxxxx
                    result.push_back(static_cast<char>(codepoint));
                } else if (codepoint <= 0x7FF) {
                    // 2字节序列: 110xxxxx 10xxxxxx
                    result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
                    result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                } else if (codepoint <= 0xFFFF) {
                    // 3字节序列: 1110xxxx 10xxxxxx 10xxxxxx
                    result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                } else if (codepoint <= 0x10FFFF) {
                    // 4字节序列: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                } else {
                    throw std::runtime_error("Unicode码点超出有效范围");
                }
            } catch (const std::exception& e) {
                // 如果发生任何错误，返回UTF-8替换字符
                return "�";
            }

            return result;
        }

        bool UnicodeProcessor::isValidCodePoint(uint32_t codepoint) {
            // 检查基本平面和补充平面的有效范围
            return codepoint <= 0x10FFFF && (codepoint < 0xD800 || codepoint > 0xDFFF) &&  // 排除代理对范围
                   (codepoint < 0xFDD0 || codepoint > 0xFDEF) &&                           // 排除非字符
                   ((codepoint & 0xFFFE) != 0xFFFE);                                       // 排除最后两个码位
        }

        std::string UnicodeProcessor::processHexEscape(const std::string& source,
                                                       size_t& currentPos,
                                                       std::string& error) {
            if (currentPos + 2 >= source.length()) {
                error = "不完整的十六进制转义序列";
                return "";
            }

            try {
                std::string hex = source.substr(currentPos, 2);
                uint32_t value = parseHexValue(hex, error);
                if (!error.empty()) {
                    return "";
                }

                if (value > 0xFF) {
                    error = "十六进制转义序列值超出范围 (0x00-0xFF): " + hex;
                    return "";
                }

                currentPos += 2;
                return std::string(1, static_cast<char>(value));
            } catch (const std::exception& e) {
                error = "处理十六进制转义序列时出错: " + std::string(e.what());
                return "";
            }
        }

        uint32_t UnicodeProcessor::parseHexValue(const std::string& hex, std::string& error) {
            uint32_t value = 0;
            for (char c : hex) {
                if (!isHexDigit(c)) {
                    error = "无效的十六进制字符: " + std::string(1, c);
                    return 0;
                }

                value = (value << 4) | hexDigitToValue(c);

                // 检查溢出
                if (value > 0x10FFFF) {
                    error = "十六进制值超出有效范围";
                    return 0;
                }
            }
            return value;
        }

        bool UnicodeProcessor::isHexDigit(char c) {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        }

        int UnicodeProcessor::hexDigitToValue(char c) {
            if (c >= '0' && c <= '9') {
                return c - '0';
            } else if (c >= 'a' && c <= 'f') {
                return c - 'a' + 10;
            } else if (c >= 'A' && c <= 'F') {
                return c - 'A' + 10;
            }
            return 0;  // 不应该到达这里，因为isHexDigit已经验证过
        }

    }  // namespace frontend
}  // namespace rp
