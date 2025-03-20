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
            // 确保当前位置是反斜杠
            if (currentPos >= source.length() || source[currentPos] != '\\') {
                error = "预期的转义序列应以'\\'开始";
                return "";
            }

            size_t originalPos = currentPos;
            currentPos++;  // 跳过反斜杠

            if (currentPos >= source.length()) {
                error = "不完整的转义序列";
                return "";
            }

            char c = source[currentPos++];

            // 处理续行
            if (c == '\n' || (c == '\r' && currentPos < source.length() && source[currentPos] == '\n')) {
                if (c == '\r') currentPos++;  // 跳过\n
                return "";                    // 返回空字符串，表示这是一个有效的续行
            }

            // 使用 Unicode 模块处理 UTF-8 字符
            if (static_cast<unsigned char>(c) >= 0x80) {
                currentPos--;  // 回退，因为这可能是一个UTF-8字符
                auto [valid, bytesRead] = StringLiteralUtils::validateUTF8Sequence(source, currentPos);
                if (valid && bytesRead > 0) {
                    auto result = source.substr(currentPos, bytesRead);
                    currentPos += bytesRead;
                    return result;
                }
                error = "无效的UTF-8序列";
                return "";
            }

            std::string result;
            switch (c) {
                case 'n':
                    result = "\n";
                    break;
                case 't':
                    result = "\t";
                    break;
                case 'r':
                    result = "\r";
                    break;
                case 'b':
                    result = "\b";
                    break;
                case 'f':
                    result = "\f";
                    break;
                case 'v':
                    result = "\v";
                    break;
                case 'a':
                    result = "\a";
                    break;
                case '\\':
                    result = "\\";
                    break;
                case '\'':
                    result = "'";
                    break;
                case '"':
                    result = "\"";
                    break;
                case '?':
                    result = "?";
                    break;
                case 'x':
                    result = processHexEscape(source, currentPos, error);
                    break;
                case 'u':
                case 'U':
                    currentPos--;  // 回退一个字符，因为processUnicodeEscape需要知道是u还是U
                    result = processUnicodeEscape(source, currentPos, error);
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    currentPos--;  // 回退一个字符，因为八进制转义序列包含第一个数字
                    result = processOctalEscape(source, currentPos, error);
                    break;
                default:
                    error = getDetailedErrorMessage("无效的转义序列 '\\" + std::string(1, c) + "'");
                    currentPos = originalPos + 2;  // 移动到转义序列之后
                    return "";
            }

            if (error.empty() && result.empty()) {
                error = "转义序列处理失败";
            }

            return result;
        }

        bool EscapeSequenceProcessor::isValidEscapeSequence(char c) {
            return c == 'n' || c == 't' || c == 'r' || c == 'b' || c == 'f' || c == 'v' || c == 'a' || c == '\\' ||
                   c == '\'' || c == '"' || c == '?' || c == 'x' || c == 'u' || c == 'U' ||
                   StringLiteralUtils::isOctalDigit(c) || c == '\n' || c == '\r';
        }

        std::string EscapeSequenceProcessor::processHexEscape(const std::string& source,
                                                              size_t& currentPos,
                                                              std::string& error) {
            if (currentPos + 2 > source.length()) {
                error = "不完整的十六进制转义序列：需要2位十六进制数";
                return "";
            }

            std::string hexStr;
            for (int i = 0; i < 2; ++i) {
                char c = source[currentPos + i];
                if (!StringLiteralUtils::isHexDigit(c)) {
                    error = "无效的十六进制转义序列：预期十六进制数字，得到 '" + std::string(1, c) + "'";
                    return "";
                }
                hexStr += c;
            }

            currentPos += 2;
            int value = 0;
            std::istringstream(hexStr) >> std::hex >> value;
            return std::string(1, static_cast<char>(value));
        }

        std::string EscapeSequenceProcessor::processOctalEscape(const std::string& source,
                                                                size_t& currentPos,
                                                                std::string& error) {
            if (!StringLiteralUtils::isOctalDigit(source[currentPos])) {
                error = "无效的八进制转义序列：预期八进制数字";
                return "";
            }

            std::string octalStr;
            int value = 0;

            // 最多读取3个八进制数字，但要确保值不超过255
            for (int i = 0; i < 3 && currentPos < source.length(); ++i) {
                char c = source[currentPos];
                if (!StringLiteralUtils::isOctalDigit(c)) {
                    break;
                }

                int nextDigit = c - '0';
                int nextValue = (value * 8) + nextDigit;

                if (nextValue > 255) {
                    break;
                }

                value = nextValue;
                octalStr += c;
                currentPos++;
            }

            if (octalStr.empty()) {
                error = "无效的八进制转义序列";
                return "";
            }

            return std::string(1, static_cast<char>(value));
        }

        std::string EscapeSequenceProcessor::processUnicodeEscape(const std::string& source,
                                                                  size_t& currentPos,
                                                                  std::string& error) {
            char escapeChar = source[currentPos++];  // 获取'u'或'U'
            size_t length = (escapeChar == 'u') ? 4 : 8;

            if (currentPos + length > source.length()) {
                error = std::string("不完整的Unicode转义序列：需要") + std::to_string(length) + "位十六进制数";
                return "";
            }

            if (!isValidUnicodeEscape(source, currentPos, length)) {
                error = "无效的Unicode转义序列：包含非十六进制字符";
                return "";
            }

            std::string hexStr = source.substr(currentPos, length);
            currentPos += length;

            // 解析Unicode码点
            uint32_t codepoint;
            std::istringstream(hexStr) >> std::hex >> codepoint;

            // 验证码点
            if (!isValidSurrogateCodePoint(codepoint)) {
                error = "无效的Unicode码点：代理项对范围 (0xD800-0xDFFF) 不允许直接使用";
                return "";
            }

            if (codepoint > 0x10FFFF) {
                error = "无效的Unicode码点：超出有效范围 (0x0-0x10FFFF)";
                return "";
            }

            // 使用Unicode模块进行UTF-8编码
            return unicode::UnicodeEncoding::encodeUtf8(codepoint);
        }

        bool EscapeSequenceProcessor::isValidUnicodeEscape(const std::string& source, size_t pos, size_t length) {
            for (size_t i = 0; i < length && pos + i < source.length(); ++i) {
                if (!StringLiteralUtils::isHexDigit(source[pos + i])) {
                    return false;
                }
            }
            return true;
        }

        std::string EscapeSequenceProcessor::getDetailedErrorMessage(const std::string& basicError) {
            return basicError +
                   "\n"
                   "有效的转义序列包括:\n"
                   "  \\n - 换行\n"
                   "  \\t - 制表符\n"
                   "  \\r - 回车\n"
                   "  \\b - 退格\n"
                   "  \\f - 换页\n"
                   "  \\v - 垂直制表符\n"
                   "  \\a - 响铃\n"
                   "  \\\\ - 反斜杠\n"
                   "  \\' - 单引号\n"
                   "  \\\" - 双引号\n"
                   "  \\? - 问号\n"
                   "  \\xHH - 十六进制转义 (HH 为两位十六进制数)\n"
                   "  \\uHHHH - Unicode转义 (HHHH 为四位十六进制数)\n"
                   "  \\UHHHHHHHH - Unicode转义 (HHHHHHHH 为八位十六进制数)\n"
                   "  \\0-\\377 - 八进制转义";
        }

    }  // namespace frontend
}  // namespace rp
