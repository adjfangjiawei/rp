#include "EscapeSequenceProcessor.h"

#include <sstream>

#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"
#include "Frontend/Parser/Lexer/Unicode/Escape/UnicodeEscape.h"
#include "Frontend/Parser/Lexer/Unicode/Processing/UnicodeProcessing.h"
#include "Frontend/Parser/Lexer/Unicode/Unicode.h"
#include "StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        std::string EscapeSequenceProcessor::processEscapeSequence(const std::string& source,
                                                                   size_t& currentPos,
                                                                   std::string& error) {
            // 边界检查
            if (currentPos >= source.length()) {
                error = "转义序列不完整";
                return "";
            }

            // 确保当前位置是反斜杠
            if (source[currentPos] != '\\') {
                error = "预期的转义序列应以'\\'开始";
                return "";
            }

            size_t originalPos = currentPos;
            currentPos++;  // 跳过反斜杠

            // 再次边界检查
            if (currentPos >= source.length()) {
                error = "转义序列不完整：反斜杠后没有字符";
                currentPos = originalPos;
                return "";
            }

            char c = source[currentPos];

            // 处理续行
            if (c == '\n' || (c == '\r' && currentPos + 1 < source.length() && source[currentPos + 1] == '\n')) {
                if (c == '\r') {
                    currentPos += 2;  // 跳过\r\n
                } else {
                    currentPos++;  // 跳过\n
                }
                return "";  // 返回空字符串表示这是一个有效的续行
            }

            // 处理UTF-8字符
            if (static_cast<unsigned char>(c) >= 0x80) {
                currentPos--;  // 回退到反斜杠之后的位置
                size_t bytesRead;
                bool isValid;
                size_t bytesConsumed;
                isValid = unicode::UnicodeProcessing::validateUtf8Sequence(source, currentPos, bytesConsumed);
                bytesRead = bytesConsumed;

                if (isValid && bytesRead > 0) {
                    std::string result = source.substr(currentPos, bytesRead);
                    currentPos += bytesRead;
                    return result;
                }

                error = "无效的UTF-8序列";
                currentPos = originalPos;
                return "";
            }

            currentPos++;  // 移动到转义字符之后

            try {
                std::string result;
                switch (c) {
                    case 'n':
                        return "\n";
                    case 't':
                        return "\t";
                    case 'r':
                        return "\r";
                    case 'b':
                        return "\b";
                    case 'f':
                        return "\f";
                    case 'v':
                        return "\v";
                    case 'a':
                        return "\a";
                    case '\\':
                        return "\\";
                    case '\'':
                        return "'";
                    case '"':
                        return "\"";
                    case '?':
                        return "?";

                    case 'x':          // 十六进制转义
                        currentPos--;  // 回退到'x'
                        return processHexEscape(source, currentPos, error);

                    case 'u':          // Unicode转义 \u
                    case 'U':          // Unicode转义 \U
                        currentPos--;  // 回退到'u'或'U'
                        return processUnicodeEscape(source, currentPos, error);

                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        currentPos--;  // 回退到八进制数字
                        return processOctalEscape(source, currentPos, error);

                    default:
                        error = getDetailedErrorMessage("无效的转义序列 '\\" + std::string(1, c) + "'");
                        return "";
                }
            } catch (const std::exception& e) {
                error = "处理转义序列时发生错误: " + std::string(e.what());
                currentPos = originalPos;
                return "";
            }
        }

        std::string EscapeSequenceProcessor::processHexEscape(const std::string& source,
                                                              size_t& currentPos,
                                                              std::string& error) {
            // 确保有足够的字符
            if (currentPos + 3 > source.length()) {  // \x + 至少一个十六进制数字
                error = "不完整的十六进制转义序列";
                return "";
            }

            currentPos++;  // 跳过'x'
            std::string hexStr;
            size_t maxDigits = 2;
            size_t originalPos = currentPos;

            // 读取最多两个十六进制数字
            for (size_t i = 0; i < maxDigits && currentPos < source.length(); ++i) {
                char c = source[currentPos];
                if (!StringLiteralUtils::isHexDigit(c)) {
                    if (i == 0) {
                        error = "十六进制转义序列后必须至少有一个十六进制数字";
                        currentPos = originalPos;
                        return "";
                    }
                    break;
                }
                hexStr += c;
                currentPos++;
            }

            try {
                unsigned long value = std::stoul(hexStr, nullptr, 16);
                if (value > 0xFF) {
                    error = "十六进制转义序列值超出范围 (0x00-0xFF)";
                    currentPos = originalPos;
                    return "";
                }
                return std::string(1, static_cast<char>(value));
            } catch (const std::exception& e) {
                error = "无效的十六进制转义序列";
                currentPos = originalPos;
                return "";
            }
        }

        std::string EscapeSequenceProcessor::processOctalEscape(const std::string& source,
                                                                size_t& currentPos,
                                                                std::string& error) {
            if (!StringLiteralUtils::isOctalDigit(source[currentPos])) {
                error = "无效的八进制转义序列：预期八进制数字";
                return "";
            }

            size_t originalPos = currentPos;
            unsigned int value = 0;
            size_t digitCount = 0;

            // 最多读取3个八进制数字
            while (digitCount < 3 && currentPos < source.length()) {
                char c = source[currentPos];
                if (!StringLiteralUtils::isOctalDigit(c)) {
                    break;
                }

                unsigned int nextDigit = c - '0';
                unsigned int nextValue = (value * 8) + nextDigit;

                if (nextValue > 0xFF) {
                    break;
                }

                value = nextValue;
                currentPos++;
                digitCount++;
            }

            if (digitCount == 0) {
                error = "无效的八进制转义序列";
                currentPos = originalPos;
                return "";
            }

            return std::string(1, static_cast<char>(value));
        }

        std::string EscapeSequenceProcessor::processUnicodeEscape(const std::string& source,
                                                                  size_t& currentPos,
                                                                  std::string& error) {
            if (currentPos >= source.length()) {
                error = "不完整的Unicode转义序列";
                return "";
            }

            size_t originalPos = currentPos;
            char escapeChar = source[currentPos++];
            size_t length = (escapeChar == 'u') ? 4 : 8;

            if (currentPos + length > source.length()) {
                error = std::string("不完整的Unicode转义序列：需要") + std::to_string(length) + "位十六进制数字";
                currentPos = originalPos;
                return "";
            }

            // 验证Unicode转义序列
            if (!isValidUnicodeEscape(source, currentPos, length)) {
                error = "无效的Unicode转义序列";
                currentPos = originalPos;
                return "";
            }

            std::string hexStr = source.substr(currentPos, length);

            try {
                uint32_t codepoint = std::stoul(hexStr, nullptr, 16);

                // 验证码点
                if (codepoint > 0x10FFFF) {
                    error = "Unicode码点超出有效范围 (0x0-0x10FFFF)";
                    currentPos = originalPos;
                    return "";
                }

                if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
                    error = "不允许使用代理对范围 (0xD800-0xDFFF) 的码点";
                    currentPos = originalPos;
                    return "";
                }

                currentPos += length;
                return unicode::UnicodeEncoding::encodeUtf8(codepoint);
            } catch (const std::exception& e) {
                error = "处理Unicode转义序列时出错: " + std::string(e.what());
                currentPos = originalPos;
                return "";
            }
        }

        bool EscapeSequenceProcessor::isValidUnicodeEscape(const std::string& source, size_t pos, size_t length) {
            // 检查是否有足够的字符
            if (pos + length > source.length()) {
                return false;
            }

            // 验证所有字符都是十六进制数字
            for (size_t i = 0; i < length; ++i) {
                if (!StringLiteralUtils::isHexDigit(source[pos + i])) {
                    return false;
                }
            }

            return true;
        }

        bool EscapeSequenceProcessor::isValidEscapeSequence(char c) {
            static const char validEscapes[] = {'n', 't', 'r', 'b', 'f', 'v', 'a', '\\', '\'', '"', '?',  'x',
                                                'u', 'U', '0', '1', '2', '3', '4', '5',  '6',  '7', '\n', '\r'};

            for (char valid : validEscapes) {
                if (c == valid) return true;
            }
            return false;
        }

        std::string EscapeSequenceProcessor::getDetailedErrorMessage(const std::string& basicError) {
            return basicError +
                   "\n有效的转义序列包括:\n"
                   "  \\n  - 换行\n"
                   "  \\t  - 制表符\n"
                   "  \\r  - 回车\n"
                   "  \\b  - 退格\n"
                   "  \\f  - 换页\n"
                   "  \\v  - 垂直制表符\n"
                   "  \\a  - 响铃\n"
                   "  \\\\  - 反斜杠\n"
                   "  \\'  - 单引号\n"
                   "  \\\"  - 双引号\n"
                   "  \\?  - 问号\n"
                   "  \\xHH       - 十六进制转义 (HH 为两位十六进制数)\n"
                   "  \\uHHHH     - Unicode转义 (HHHH 为四位十六进制数)\n"
                   "  \\UHHHHHHHH - Unicode转义 (HHHHHHHH 为八位十六进制数)\n"
                   "  \\0-\\377    - 八进制转义";
        }

    }  // namespace frontend
}  // namespace rp
