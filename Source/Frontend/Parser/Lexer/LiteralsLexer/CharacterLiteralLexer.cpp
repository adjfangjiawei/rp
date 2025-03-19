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

            // 检查直接结束的情况
            if (input[pos] == '\'') {
                error = "Empty character literal";
                pos = startPos + 2;  // 跳过两个引号
                return false;
            }

            // 处理字符内容
            if (input[pos] == '\\') {
                // 处理转义序列
                pos++;
                if (pos >= inputLength) {
                    error = "Unexpected end of input after backslash";
                    pos = startPos + 2;
                    return false;
                }

                std::string remaining(input.data() + pos - 1, input.length() - (pos - 1));
                auto result = unicode::UnicodeEscape::parseEscapeSequence(remaining, 0);

                if (!result.success) {
                    error = result.error;
                    pos = startPos + 2;
                    return false;
                }

                // 确保码点值在单个字符的范围内
                if (result.codepoint > 0xFF) {
                    error = "Character literal value too large";
                    pos = startPos + result.consumed + 1;
                    return false;
                }

                value = static_cast<unsigned char>(result.codepoint);
                pos += result.consumed - 1;  // -1是因为我们已经跳过了反斜杠
            } else {
                // 处理普通字符或UTF-8字符
                auto charResult = unicode::UnicodeProcessing::processCharacter(input, pos);
                if (!charResult.success) {
                    error = charResult.error;
                    pos = startPos + 1;
                    return false;
                }

                value = charResult.value;
                pos += charResult.consumed;
            }

            // 检查结束单引号
            if (pos >= inputLength || input[pos] != '\'') {
                error = "Character literal missing closing quote";
                pos = startPos + 1;
                return false;
            }
            pos++;  // 跳过结束引号

            return true;
        }

        bool CharacterLiteralLexer::validateCharacterLiteral(const std::string &str, std::string &error) {
            // 基本长度检查
            if (str.empty()) {
                error = "Empty character literal";
                return false;
            }

            if (str.length() < 3) {
                error = "Character literal too short (minimum length is 3: 'x')";
                return false;
            }

            // 检查引号
            if (str[0] != '\'' || str[str.length() - 1] != '\'') {
                error = "Character literal must be enclosed in single quotes";
                return false;
            }

            // 检查内容长度
            size_t contentLength = str.length() - 2;  // 减去两个引号
            if (contentLength == 0) {
                error = "Empty character literal";
                return false;
            }

            // 提取字符内容
            std::string content = str.substr(1, contentLength);

            if (content[0] == '\\') {
                // 验证转义序列
                return unicode::UnicodeEscape::validateEscapeSequence(content, error);
            } else {
                // 验证UTF-8字符
                size_t bytesConsumed;
                if (!unicode::UnicodeProcessing::validateUtf8Sequence(content, 0, bytesConsumed)) {
                    error = "Invalid UTF-8 sequence";
                    return false;
                }

                // 确保只有一个字符
                if (bytesConsumed != content.length()) {
                    error = "Character literal can only contain one character";
                    return false;
                }
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
