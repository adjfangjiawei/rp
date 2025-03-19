#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"

#include <cctype>
#include <sstream>

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"

namespace rp {
    namespace frontend {

        bool StringLiteralLexer::processStringLiteral(const std::string &input,
                                                      std::string &output,
                                                      size_t &consumed,
                                                      std::string &error) {
            if (input.empty() || input[0] != '"') {
                error = "String literal must start with double quote";
                return false;
            }

            size_t pos = 1;  // 跳过开始的双引号
            output.clear();
            const size_t inputLength = input.length();

            // 添加最大字符串长度限制，防止无限循环
            const size_t MAX_STRING_LENGTH = 1024 * 1024;  // 1MB
            size_t currentLength = 0;

            while (pos < inputLength) {
                if (currentLength > MAX_STRING_LENGTH) {
                    error = "String literal too long (maximum length is 1MB)";
                    return false;
                }

                char c = input[pos];

                if (c == '"') {
                    // 字符串结束
                    consumed = pos + 1;
                    return true;
                } else if (c == '\\') {
                    // 处理转义序列
                    if (pos + 1 >= inputLength) {
                        error = "Incomplete escape sequence";
                        return false;
                    }

                    pos++;  // 移动到转义字符
                    char escaped = input[pos];

                    switch (escaped) {
                        case 'n':
                            output += '\n';
                            break;
                        case 't':
                            output += '\t';
                            break;
                        case 'r':
                            output += '\r';
                            break;
                        case '\\':
                            output += '\\';
                            break;
                        case '"':
                            output += '"';
                            break;
                        case '\'':
                            output += '\'';
                            break;
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                            {
                                // 处理八进制转义序列
                                size_t octLen = 1;
                                char value = escaped - '0';

                                // 最多读取3位八进制数
                                while (octLen < 3 && pos + 1 < inputLength) {
                                    char next = input[pos + 1];
                                    if (next >= '0' && next <= '7') {
                                        value = (value << 3) | (next - '0');
                                        pos++;
                                        octLen++;
                                    } else {
                                        break;
                                    }
                                }
                                output += value;
                                break;
                            }
                        case 'x':
                            {
                                // 处理十六进制转义序列
                                if (pos + 2 >= inputLength) {
                                    error = "Incomplete hex escape sequence";
                                    return false;
                                }

                                // 验证接下来的两个字符是否都是有效的十六进制数字
                                char hex1 = input[pos + 1];
                                char hex2 = input[pos + 2];
                                if (!std::isxdigit(hex1) || !std::isxdigit(hex2)) {
                                    error = "Invalid hex escape sequence: \\x" + std::string(1, hex1) +
                                            std::string(1, hex2);
                                    return false;
                                }

                                try {
                                    std::string hex = input.substr(pos + 1, 2);
                                    int value = std::stoi(hex, nullptr, 16);
                                    output += static_cast<char>(value);
                                    pos += 2;
                                } catch (const std::exception &e) {
                                    error = "Invalid hex escape sequence";
                                    return false;
                                }
                                break;
                            }
                        case 'u':
                        case 'U':
                            {
                                // 处理Unicode转义序列
                                bool isLongForm = (escaped == 'U');
                                if (pos + (isLongForm ? 8 : 4) >= inputLength) {
                                    error = std::string("Incomplete Unicode escape sequence: \\") + escaped;
                                    return false;
                                }

                                try {
                                    auto result = unicode::UnicodeEscape::parseEscapeSequence(input, pos - 1);
                                    if (!result.success) {
                                        error = result.error;
                                        return false;
                                    }

                                    // 验证码点范围
                                    if (result.codepoint > 0x10FFFF) {
                                        error = "Unicode code point out of range";
                                        return false;
                                    }

                                    output += unicode::UnicodeEncoding::codePointToUtf8(result.codepoint);
                                    pos += result.consumed - 1;
                                } catch (const std::exception &e) {
                                    error = std::string("Invalid Unicode escape sequence: ") + e.what();
                                    return false;
                                }
                                break;
                            }
                        default:
                            error = "Invalid escape sequence: \\" + std::string(1, escaped);
                            return false;
                    }
                } else if (c == '\n' || c == '\r') {
                    error = "Unterminated string literal: new line in string literal";
                    return false;
                } else if (static_cast<unsigned char>(c) < 0x20) {
                    // 检查不可打印的控制字符
                    error = "Invalid control character in string literal";
                    return false;
                } else {
                    // 处理普通字符或UTF-8序列
                    currentLength++;
                    auto result = unicode::UnicodeProcessing::processUtf8Character(input, pos);
                    if (!result.success) {
                        error = result.error;
                        return false;
                    }
                    output += input.substr(pos, result.consumed);
                    pos += result.consumed - 1;
                }
                pos++;
            }

            error = "Unterminated string literal";
            return false;
        }

        bool StringLiteralLexer::processRawStringLiteral(const std::string &input,
                                                         std::string &output,
                                                         size_t &consumed,
                                                         std::string &error) {
            // 检查R"开头
            if (input.length() < 2 || input[0] != 'R' || input[1] != '"') {
                error = "Raw string literal must start with R\"";
                return false;
            }

            size_t pos = 2;  // 跳过R"
            const size_t inputLength = input.length();

            // 查找分隔符
            std::string delimiter;
            while (pos < inputLength && input[pos] != '(') {
                char c = input[pos];
                if (!std::isalnum(c) && c != '_') {
                    error = "Invalid character in raw string delimiter";
                    return false;
                }
                delimiter += c;
                pos++;
                if (delimiter.length() > 16) {
                    error = "Raw string delimiter too long (maximum 16 characters)";
                    return false;
                }
            }

            if (pos >= inputLength || input[pos] != '(') {
                error = "Expected '(' after delimiter in raw string";
                return false;
            }
            pos++;  // 跳过'('

            // 添加最大字符串长度限制
            const size_t MAX_RAW_STRING_LENGTH = 1024 * 1024;  // 1MB

            // 查找结束序列 )delimiter"
            std::string endSequence = ")" + delimiter + "\"";
            size_t contentStart = pos;
            size_t contentEnd = input.find(endSequence, contentStart);

            if (contentEnd == std::string::npos) {
                error = "Unterminated raw string literal: missing )" + delimiter + "\"";
                return false;
            }

            // 检查字符串长度
            size_t contentLength = contentEnd - contentStart;
            if (contentLength > MAX_RAW_STRING_LENGTH) {
                error = "Raw string literal too long (maximum length is 1MB)";
                return false;
            }

            // 检查字符串内容中是否有非法字符
            for (size_t i = contentStart; i < contentEnd; ++i) {
                if (static_cast<unsigned char>(input[i]) < 0x20 && input[i] != '\n' && input[i] != '\r' &&
                    input[i] != '\t') {
                    error = "Invalid control character in raw string literal";
                    return false;
                }
            }

            // 提取原始字符串内容
            output = input.substr(contentStart, contentLength);
            consumed = contentEnd + endSequence.length();

            // 验证UTF-8编码
            if (!unicode::UnicodeProcessing::validateUtf8String(output, error)) {
                return false;
            }

            return true;
        }

        bool StringLiteralLexer::validateStringLiteral(const std::string &str, std::string &error) {
            if (str.empty()) {
                return true;  // 空字符串是合法的
            }

            // 验证字符串是否是有效的UTF-8
            return unicode::UnicodeProcessing::validateUtf8String(str, error);
        }

    }  // namespace frontend
}  // namespace rp
