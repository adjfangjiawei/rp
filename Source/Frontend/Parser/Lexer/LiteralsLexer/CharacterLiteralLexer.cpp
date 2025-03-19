#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"

#include <cctype>
#include <sstream>

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"

namespace rp {
    namespace frontend {

        bool CharacterLiteralLexer::processCharacterLiteral(const std::string &input,
                                                            size_t &pos,
                                                            long long &value,
                                                            std::string &error) {
            // 初始化和基本检查
            if (input.empty()) {
                error = "Empty input for character literal";
                pos = 0;
                return false;
            }

            const size_t startPos = pos;
            const size_t inputLength = input.length();

            // 检查起始单引号
            if (pos >= inputLength || input[pos] != '\'') {
                error = "Character literal must start with a single quote";
                pos = startPos + 1;  // 确保至少前进一个位置
                return false;
            }
            pos++;

            // 检查空字符字面量
            if (pos >= inputLength) {
                error = "Unexpected end of input after opening quote";
                pos = startPos + 1;
                return false;
            }

            // 检查直接结束的情况（空字符字面量）
            if (input[pos] == '\'') {
                error = "Empty character literal";
                pos = startPos + 2;  // 跳过两个引号
                value = 0;           // 设置空字符的值为0
                return false;
            }

            bool isEscapeSequence = false;

            // 处理字符内容
            if (input[pos] == '\\') {
                isEscapeSequence = true;
                // 处理转义序列
                pos++;
                if (pos >= inputLength) {
                    error = "Unexpected end of input after backslash";
                    pos = startPos + 2;
                    return false;
                }

                char escaped = input[pos];
                switch (escaped) {
                    case 'n':
                        value = '\n';
                        pos++;
                        break;
                    case 't':
                        value = '\t';
                        pos++;
                        break;
                    case 'r':
                        value = '\r';
                        pos++;
                        break;
                    case '\\':
                        value = '\\';
                        pos++;
                        break;
                    case '\'':
                        value = '\'';
                        pos++;
                        break;
                    case '"':
                        value = '"';
                        pos++;
                        break;
                    case 'a':
                        value = '\a';
                        pos++;
                        break;
                    case 'b':
                        value = '\b';
                        pos++;
                        break;
                    case 'f':
                        value = '\f';
                        pos++;
                        break;
                    case 'v':
                        value = '\v';
                        pos++;
                        break;
                    case 'x':
                        {
                            // 处理十六进制转义序列
                            if (pos + 2 >= inputLength) {
                                error = "Incomplete hex escape sequence";
                                pos = startPos + 2;
                                return false;
                            }

                            char hex1 = input[pos + 1];
                            char hex2 = input[pos + 2];
                            if (!std::isxdigit(hex1) || !std::isxdigit(hex2)) {
                                error = "Invalid hex escape sequence";
                                pos = startPos + 2;
                                return false;
                            }

                            std::string hex = input.substr(pos + 1, 2);
                            value = std::stoi(hex, nullptr, 16);
                            pos += 3;
                            break;
                        }
                    case 'u':
                    case 'U':
                        {
                            // 处理Unicode转义序列
                            std::string remaining(input.data() + pos - 1, input.length() - (pos - 1));
                            auto result = unicode::UnicodeEscape::parseEscapeSequence(remaining, 0);

                            if (!result.success) {
                                error = result.error;
                                pos = startPos + 2;
                                return false;
                            }

                            // 确保码点值在合理范围内
                            if (result.codepoint > 0x10FFFF) {
                                error = "Unicode code point out of range";
                                pos = startPos + result.consumed + 1;
                                return false;
                            }

                            value = result.codepoint;
                            pos += result.consumed - 1;
                            break;
                        }
                    default:
                        if (escaped >= '0' && escaped <= '7') {
                            // 处理八进制转义序列
                            value = escaped - '0';
                            pos++;

                            // 最多读取3位八进制数
                            for (int i = 0; i < 2 && pos < inputLength; i++) {
                                char next = input[pos];
                                if (next >= '0' && next <= '7') {
                                    value = (value << 3) | (next - '0');
                                    pos++;
                                } else {
                                    break;
                                }
                            }
                        } else {
                            error = "Invalid escape sequence: \\" + std::string(1, escaped);
                            pos = startPos + 2;
                            return false;
                        }
                }
            } else {
                // 处理普通字符或UTF-8字符
                size_t utf8Start = pos;
                size_t bytesConsumed;
                std::string content;

                // 读取直到结束引号或文件结束
                while (pos < inputLength && input[pos] != '\'') {
                    content += input[pos];
                    pos++;
                }

                // 验证UTF-8序列
                if (!unicode::UnicodeProcessing::validateUtf8Sequence(content, 0, bytesConsumed)) {
                    error = "Invalid UTF-8 sequence";
                    return false;
                }

                // 更新value为第一个UTF-8字符的码点值
                auto charResult = unicode::UnicodeProcessing::processCharacter(content, 0);
                if (!charResult.success) {
                    error = charResult.error;
                    return false;
                }
                value = charResult.value;
            }

            // 检查结束单引号
            if (pos >= inputLength || input[pos] != '\'') {
                error = "Character literal missing closing quote";
                // 设置位置到最后一个有效字符之后
                pos = std::min(pos + 1, inputLength);
                return false;
            }
            pos++;  // 跳过结束引号

            // 检查字符字面量长度
            size_t contentLength = pos - startPos - 2;     // 减去两个引号
            if (!isEscapeSequence && contentLength > 4) {  // UTF-8字符最多4字节
                error = "Character literal too long";
                return false;
            }

            return true;
        }

        bool CharacterLiteralLexer::validateCharacterLiteral(const std::string &str, std::string &error) {
            // 基本长度检查
            if (str.empty()) {
                error = "Empty character literal";
                return false;
            }

            // 检查引号
            if (str[0] != '\'' || str[str.length() - 1] != '\'') {
                error = "Character literal must be enclosed in single quotes";
                return false;
            }

            // 检查内容长度
            size_t contentLength = str.length() - 2;  // 减去两个引号

            // 不允许空字符字面量
            if (contentLength == 0) {
                error = "Empty character literal";
                return false;
            }

            // 提取字符内容
            std::string content = str.substr(1, contentLength);

            if (content[0] == '\\') {
                // 验证转义序列
                if (!unicode::UnicodeEscape::validateEscapeSequence(content, error)) {
                    return false;
                }
            } else {
                // 验证UTF-8字符
                size_t bytesConsumed;
                if (!unicode::UnicodeProcessing::validateUtf8Sequence(content, 0, bytesConsumed)) {
                    error = "Invalid UTF-8 sequence";
                    return false;
                }

                // 检查是否只有一个UTF-8字符
                if (bytesConsumed != contentLength) {
                    error = "Character literal can only contain one character";
                    return false;
                }
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
