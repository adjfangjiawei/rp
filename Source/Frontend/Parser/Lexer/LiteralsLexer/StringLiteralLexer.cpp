
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"

#include <cctype>
#include <sstream>

namespace rp {
    namespace frontend {

        std::string StringLiteralLexer::processEscapeSequences(const std::string &raw) {
            std::string result;
            size_t pos = 0;

            while (pos < raw.length()) {
                char c = raw[pos];

                if (c == '\\') {
                    pos++;
                    if (pos >= raw.length()) {
                        break;
                    }

                    switch (raw[pos]) {
                        case 'n':
                            result += '\n';
                            break;
                        case 't':
                            result += '\t';
                            break;
                        case 'r':
                            result += '\r';
                            break;
                        case 'b':
                            result += '\b';
                            break;
                        case 'f':
                            result += '\f';
                            break;
                        case 'v':
                            result += '\v';
                            break;
                        case 'a':
                            result += '\a';
                            break;
                        case '\\':
                            result += '\\';
                            break;
                        case '\'':
                            result += '\'';
                            break;
                        case '"':
                            result += '"';
                            break;
                        case '0':
                            result += '\0';
                            break;
                        case 'u':  // Unicode转义序列
                            pos++;
                            if (!processUnicodeEscape(raw, pos, result)) {
                                return "";
                            }
                            pos--;  // 因为外层循环会pos++
                            break;
                        case 'x':
                            {  // 十六进制转义序列
                                pos++;
                                if (pos + 1 >= raw.length()) return "";
                                char hex[3] = {raw[pos], raw[pos + 1], 0};
                                char value = static_cast<char>(std::stoi(hex, nullptr, 16));
                                result += value;
                                pos += 1;
                                break;
                            }
                        default:
                            // 无效的转义序列
                            return "";
                    }
                } else {
                    result += c;
                }
                pos++;
            }

            return result;
        }

        bool StringLiteralLexer::validateStringLiteral(const std::string &str, std::string &error) {
            // 检查未闭合的引号
            int quoteCount = 0;
            for (char c : str) {
                if (c == '"') quoteCount++;
            }

            if (quoteCount % 2 != 0) {
                error = "Unclosed string literal";
                return false;
            }

            // 检查非法字符
            for (size_t i = 0; i < str.length(); i++) {
                char c = str[i];
                if (c == '\n' || c == '\r') {
                    error = "Unterminated string literal";
                    return false;
                }
            }

            return true;
        }

        bool StringLiteralLexer::processRawStringLiteral(const std::string &input,
                                                         std::string &output,
                                                         std::string &error) {
            // R"delim(raw_characters)delim"
            if (input.length() < 4 || input[0] != 'R' || input[1] != '"') {
                error = "Invalid raw string literal format";
                return false;
            }

            // 查找分隔符
            size_t delimStart = 2;
            size_t delimEnd = input.find('(', delimStart);
            if (delimEnd == std::string::npos) {
                error = "Missing opening parenthesis in raw string";
                return false;
            }

            std::string delim = input.substr(delimStart, delimEnd - delimStart);
            if (delim.length() > 16) {
                error = "Raw string delimiter too long";
                return false;
            }

            // 查找结束位置
            std::string endDelim = ")" + delim + "\"";
            size_t contentStart = delimEnd + 1;
            size_t contentEnd = input.find(endDelim, contentStart);
            if (contentEnd == std::string::npos) {
                error = "Unterminated raw string literal";
                return false;
            }

            output = input.substr(contentStart, contentEnd - contentStart);
            return true;
        }

        bool StringLiteralLexer::processEscapeSequence(const std::string &input, size_t &pos, std::string &output) {
            if (pos >= input.length()) return false;

            char c = input[pos];
            switch (c) {
                case 'n':
                    output += '\n';
                    break;
                case 't':
                    output += '\t';
                    break;
                case 'r':
                    output += '\r';
                    break;
                case 'b':
                    output += '\b';
                    break;
                case 'f':
                    output += '\f';
                    break;
                case 'v':
                    output += '\v';
                    break;
                case 'a':
                    output += '\a';
                    break;
                case '\\':
                    output += '\\';
                    break;
                case '\'':
                    output += '\'';
                    break;
                case '"':
                    output += '"';
                    break;
                case '0':
                    output += '\0';
                    break;
                case 'u':
                    return processUnicodeEscape(input, pos, output);
                case 'x':
                    {
                        if (pos + 2 >= input.length()) return false;
                        std::string hex = input.substr(pos + 1, 2);
                        char value = static_cast<char>(std::stoi(hex, nullptr, 16));
                        output += value;
                        pos += 2;
                        break;
                    }
                default:
                    return false;
            }

            return true;
        }

        bool StringLiteralLexer::processUnicodeEscape(const std::string &input, size_t &pos, std::string &output) {
            if (pos + 4 >= input.length()) return false;

            // 读取4位十六进制数
            std::string hex = input.substr(pos, 4);
            for (char c : hex) {
                if (!std::isxdigit(c)) return false;
            }

            // 转换为Unicode码点
            unsigned int codePoint = std::stoul(hex, nullptr, 16);

            // 转换为UTF-8编码
            if (codePoint < 0x80) {
                output += static_cast<char>(codePoint);
            } else if (codePoint < 0x800) {
                output += static_cast<char>((codePoint >> 6) | 0xC0);
                output += static_cast<char>((codePoint & 0x3F) | 0x80);
            } else {
                output += static_cast<char>((codePoint >> 12) | 0xE0);
                output += static_cast<char>(((codePoint >> 6) & 0x3F) | 0x80);
                output += static_cast<char>((codePoint & 0x3F) | 0x80);
            }

            pos += 3;  // pos会在外部再++
            return true;
        }

    }  // namespace frontend
}  // namespace rp
