
#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"

#include <cctype>
#include <sstream>

namespace rp {
    namespace frontend {

        bool CharacterLiteralLexer::processCharacterLiteral(const std::string &input,
                                                            size_t &pos,
                                                            long long &value,
                                                            std::string &error) {
            // 检查起始单引号
            if (input.empty() || input[pos] != '\'') {
                error = "Expected single quote at start of character literal";
                return false;
            }
            pos++;

            // 空字符字面量
            if (pos >= input.length()) {
                error = "Empty character literal";
                return false;
            }

            // 处理字符内容
            if (input[pos] == '\\') {
                // 处理转义序列
                pos++;
                if (!processEscapeSequence(input, pos, value)) {
                    error = "Invalid escape sequence";
                    return false;
                }
            } else if (static_cast<unsigned char>(input[pos]) >= 0x80) {
                // 处理Unicode字符
                if (!processUnicodeChar(input, pos, value)) {
                    error = "Invalid Unicode character";
                    return false;
                }
            } else {
                // 处理普通ASCII字符
                value = static_cast<long long>(input[pos]);
                pos++;
            }

            // 检查结束单引号
            if (pos >= input.length() || input[pos] != '\'') {
                error = "Expected single quote at end of character literal";
                return false;
            }
            pos++;

            return true;
        }

        bool CharacterLiteralLexer::validateCharacterLiteral(const std::string &str, std::string &error) {
            // 检查基本格式
            if (str.length() < 3) {  // 最少需要3个字符：'x'
                error = "Character literal too short";
                return false;
            }

            if (str[0] != '\'' || str[str.length() - 1] != '\'') {
                error = "Character literal must be enclosed in single quotes";
                return false;
            }

            // 检查字符数量（不包括引号）
            size_t contentLength = str.length() - 2;
            if (contentLength > 4) {  // UTF-8最多4字节
                error = "Character literal too long";
                return false;
            }

            return true;
        }

        bool CharacterLiteralLexer::processEscapeSequence(const std::string &input, size_t &pos, long long &value) {
            if (pos >= input.length()) return false;

            char c = input[pos];
            switch (c) {
                case 'n':
                    value = '\n';
                    break;
                case 't':
                    value = '\t';
                    break;
                case 'r':
                    value = '\r';
                    break;
                case 'b':
                    value = '\b';
                    break;
                case 'f':
                    value = '\f';
                    break;
                case 'v':
                    value = '\v';
                    break;
                case 'a':
                    value = '\a';
                    break;
                case '\\':
                    value = '\\';
                    break;
                case '\'':
                    value = '\'';
                    break;
                case '"':
                    value = '"';
                    break;
                case '0':
                    value = '\0';
                    break;
                case 'u':  // Unicode转义序列
                    pos++;
                    return processUnicodeChar(input, pos, value);
                case 'x':
                    {  // 十六进制转义序列
                        if (pos + 2 >= input.length()) return false;
                        std::string hex = input.substr(pos + 1, 2);
                        try {
                            value = std::stoll(hex, nullptr, 16);
                        } catch (...) {
                            return false;
                        }
                        pos += 2;
                        break;
                    }
                default:
                    return false;
            }

            pos++;
            return true;
        }

        bool CharacterLiteralLexer::processUnicodeChar(const std::string &input, size_t &pos, long long &value) {
            if (pos + 4 >= input.length()) return false;

            // 读取4位十六进制数
            std::string hex = input.substr(pos, 4);
            for (char c : hex) {
                if (!std::isxdigit(c)) return false;
            }

            // 转换为Unicode码点
            unsigned int codepoint;
            try {
                codepoint = std::stoul(hex, nullptr, 16);
            } catch (...) {
                return false;
            }

            // 转换为UTF-8编码
            if (!processUtf8Encoding(codepoint, value)) {
                return false;
            }

            pos += 3;  // pos会在外部再++
            return true;
        }

        bool CharacterLiteralLexer::processUtf8Encoding(unsigned int codepoint, long long &value) {
            // UTF-8编码规则
            if (codepoint < 0x80) {
                // 单字节UTF-8
                value = codepoint;
            } else if (codepoint < 0x800) {
                // 双字节UTF-8
                value = ((codepoint >> 6) | 0xC0) << 8;
                value |= ((codepoint & 0x3F) | 0x80);
            } else if (codepoint < 0x10000) {
                // 三字节UTF-8
                value = ((codepoint >> 12) | 0xE0) << 16;
                value |= (((codepoint >> 6) & 0x3F) | 0x80) << 8;
                value |= ((codepoint & 0x3F) | 0x80);
            } else if (codepoint < 0x110000) {
                // 四字节UTF-8
                value = ((codepoint >> 18) | 0xF0) << 24;
                value |= (((codepoint >> 12) & 0x3F) | 0x80) << 16;
                value |= (((codepoint >> 6) & 0x3F) | 0x80) << 8;
                value |= ((codepoint & 0x3F) | 0x80);
            } else {
                return false;  // 无效的Unicode码点
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
