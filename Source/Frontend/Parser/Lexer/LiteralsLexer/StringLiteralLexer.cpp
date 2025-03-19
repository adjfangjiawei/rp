#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"

#include <cctype>
#include <sstream>

#include "Frontend/Parser/Lexer/Utils/Unicode.h"
#include "Frontend/Parser/Lexer/Utils/UnicodeEscape.h"

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
            bool isMultiline = false;

            // 检查是否是多行字符串
            if (pos + 1 < inputLength && input[pos] == '"' && input[pos + 1] == '"') {
                isMultiline = true;
                pos += 2;
                // 如果多行字符串开始后直接是换行，跳过第一个换行
                if (pos < inputLength && input[pos] == '\n') {
                    pos++;
                } else if (pos + 1 < inputLength && input[pos] == '\r' && input[pos + 1] == '\n') {
                    pos += 2;
                }
            }

            while (pos < inputLength) {
                char c = input[pos];

                if (c == '"') {
                    if (isMultiline) {
                        // 检查是否是多行字符串的结束 """
                        if (pos + 2 < inputLength && input[pos + 1] == '"' && input[pos + 2] == '"') {
                            consumed = pos + 3;
                            return true;
                        }
                        output += c;
                        pos++;
                    } else {
                        // 普通字符串结束
                        consumed = pos + 1;
                        return true;
                    }
                } else if (c == '\\') {
                    // 处理转义序列
                    std::string_view remaining(input.data() + pos, input.length() - pos);

                    if (isMultiline) {
                        // 处理行连续
                        auto lineContResult = UnicodeEscape::processLineContinuation(remaining);
                        if (lineContResult.success) {
                            pos += lineContResult.consumed;
                            continue;
                        }
                    }

                    auto result = UnicodeEscape::parseEscapeSequence(remaining);
                    if (!result.success) {
                        error = result.error;
                        return false;
                    }

                    output += result.value;
                    pos += result.consumed;
                } else if (!isMultiline && (c == '\n' || c == '\r')) {
                    error = "Unterminated string literal";
                    return false;
                } else {
                    // 处理普通字符或UTF-8序列
                    auto charResult = Unicode::processUtf8Character(input, pos);
                    if (!charResult.success) {
                        error = charResult.error;
                        return false;
                    }

                    output += input.substr(pos, charResult.consumed);
                    pos += charResult.consumed;
                }
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
                // 分隔符只能包含: [a-zA-Z0-9_]
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

            // 查找结束序列 )delimiter"
            std::string endSequence = ")" + delimiter + "\"";
            size_t contentStart = pos;
            size_t contentEnd = input.find(endSequence, contentStart);

            if (contentEnd == std::string::npos) {
                error = "Unterminated raw string literal";
                return false;
            }

            // 提取原始字符串内容
            size_t contentLength = contentEnd - contentStart;
            auto result = Unicode::processUtf8String(input, contentStart, contentLength);

            if (!result.success) {
                error = result.error;
                return false;
            }

            output = std::move(result.value);
            consumed = contentEnd + endSequence.length();
            return true;
        }

        bool StringLiteralLexer::validateStringLiteral(const std::string &str, std::string &error) {
            if (str.empty()) {
                return true;  // 空字符串是合法的
            }

            // 对于原始字符串字面量，只需要验证UTF-8编码
            if (str.length() >= 2 && str[0] == 'R' && str[1] == '"') {
                return Unicode::validateUtf8String(str, error);
            }

            // 对于普通字符串字面量，需要验证转义序列
            auto result = UnicodeEscape::unescapeString(str);
            if (!result.success) {
                error = result.error;
                return false;
            }

            // 验证解析后的字符串是否是有效的UTF-8
            return Unicode::validateUtf8String(result.value, error);
        }

    }  // namespace frontend
}  // namespace rp
