#include "RawStringProcessor.h"

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/StringLiteralUtils.h"
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"
namespace rp {
    namespace frontend {

        Token RawStringProcessor::processRawStringLiteral(const std::string& source,
                                                          size_t& currentPos,
                                                          const SourceLocation& startLoc) {
            // 检查原始字符串的开始标记 'R"'
            if (currentPos >= source.length()) {
                Token token(TokenKind::Invalid);
                token.setError("原始字符串不完整",
                               static_cast<unsigned int>(startLoc.line),
                               static_cast<unsigned int>(startLoc.column));
                return token;
            }

            // 当前位置应该是双引号，因为 'R' 已经被前面的代码处理
            if (source[currentPos] != '"') {
                Token token(TokenKind::Invalid);
                token.setError("原始字符串必须以 R\" 开始",
                               static_cast<unsigned int>(startLoc.line),
                               static_cast<unsigned int>(startLoc.column));
                return token;
            }
            currentPos++;  // 跳过双引号

            // 查找分隔符和开始的括号
            std::string delimiter;
            bool foundOpenParen = false;
            const size_t MAX_DELIMITER_LENGTH = 16;

            // 收集分隔符（允许空分隔符）
            while (currentPos < source.length()) {
                char c = source[currentPos];

                if (c == '(') {
                    foundOpenParen = true;
                    currentPos++;
                    break;
                }

                // 检查分隔符字符的有效性
                if (!isValidDelimiter(std::string(1, c))) {
                    Token token(TokenKind::Invalid);
                    token.setError("无效的原始字符串分隔符字符: '" + std::string(1, c) + "'",
                                   static_cast<unsigned int>(startLoc.line),
                                   static_cast<unsigned int>(startLoc.column));
                    return token;
                }

                delimiter += c;
                currentPos++;

                if (delimiter.length() > MAX_DELIMITER_LENGTH) {
                    Token token(TokenKind::Invalid);
                    token.setError("原始字符串分隔符过长（最大长度为" + std::to_string(MAX_DELIMITER_LENGTH) + "）",
                                   static_cast<unsigned int>(startLoc.line),
                                   static_cast<unsigned int>(startLoc.column));
                    return token;
                }
            }

            if (!foundOpenParen) {
                Token token(TokenKind::Invalid);
                token.setError("原始字符串缺少开始括号",
                               static_cast<unsigned int>(startLoc.line),
                               static_cast<unsigned int>(startLoc.column));
                return token;
            }

            // 收集字符串内容
            std::string content;
            bool foundClosingSequence = false;
            std::string closingSequence = ")" + delimiter + "\"";
            size_t lineCount = 0;
            const size_t MAX_LINES = 1000;  // 限制最大行数

            while (currentPos < source.length()) {
                // 检查结束序列
                if (source[currentPos] == ')') {
                    size_t tempPos = currentPos;
                    bool isClosing = true;
                    size_t remainingLength = source.length() - tempPos;

                    if (remainingLength >= closingSequence.length()) {
                        // 尝试匹配完整的结束序列
                        for (size_t i = 0; i < closingSequence.length(); ++i) {
                            if (source[tempPos + i] != closingSequence[i]) {
                                isClosing = false;
                                break;
                            }
                        }

                        if (isClosing) {
                            // 验证结束序列后没有紧跟着分隔符的有效字符
                            size_t afterClosing = tempPos + closingSequence.length();
                            if (afterClosing < source.length()) {
                                char nextChar = source[afterClosing];
                                if (isValidDelimiter(std::string(1, nextChar))) {
                                    isClosing = false;
                                }
                            }

                            if (isClosing) {
                                foundClosingSequence = true;
                                currentPos = tempPos + closingSequence.length();
                                break;
                            }
                        }
                    }
                }

                // 处理换行符
                if (source[currentPos] == '\n' || source[currentPos] == '\r') {
                    lineCount++;
                    if (lineCount > MAX_LINES) {
                        Token token(TokenKind::Invalid);
                        token.setError("原始字符串超过最大行数限制（" + std::to_string(MAX_LINES) + "行）",
                                       static_cast<unsigned int>(startLoc.line),
                                       static_cast<unsigned int>(startLoc.column));
                        return token;
                    }

                    // 统一处理换行符
                    if (source[currentPos] == '\r' && currentPos + 1 < source.length() &&
                        source[currentPos + 1] == '\n') {
                        content += '\n';  // 将 CRLF 转换为 LF
                        currentPos += 2;
                    } else {
                        content += '\n';  // 将 CR 或 LF 转换为 LF
                        currentPos++;
                    }
                    continue;
                }

                // 处理 UTF-8 字符
                unsigned char currentChar = static_cast<unsigned char>(source[currentPos]);
                if (currentChar >= 0x80) {
                    auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);
                    if (length == 0) {
                        Token token(TokenKind::Invalid);
                        token.setError("无效的UTF-8序列",
                                       static_cast<unsigned int>(startLoc.line),
                                       static_cast<unsigned int>(startLoc.column));
                        return token;
                    }

                    content.append(source.substr(currentPos, length));
                    currentPos += length;
                    continue;
                }

                // 处理普通字符
                content += source[currentPos];
                currentPos++;
            }

            if (!foundClosingSequence) {
                Token token(TokenKind::Invalid);
                token.setError("原始字符串未找到匹配的结束序列 )" + delimiter + "\"",
                               static_cast<unsigned int>(startLoc.line),
                               static_cast<unsigned int>(startLoc.column));
                return token;
            }

            // 验证最终内容的 UTF-8 编码
            size_t validatePos = 0;
            while (validatePos < content.length()) {
                auto [valid, bytesRead] = StringLiteralUtils::validateUTF8Sequence(content, validatePos);
                if (!valid || bytesRead == 0) {
                    Token token(TokenKind::Invalid);
                    token.setError("字符串内容包含无效的UTF-8序列",
                                   static_cast<unsigned int>(startLoc.line),
                                   static_cast<unsigned int>(startLoc.column));
                    return token;
                }
                validatePos += bytesRead;
            }

            Token token(TokenKind::StringLiteral);
            token.setText(content);
            token.line = static_cast<unsigned int>(startLoc.line);
            token.column = static_cast<unsigned int>(startLoc.column);
            return token;
        }

        bool RawStringProcessor::isValidDelimiter(const std::string& delimiter) {
            for (char c : delimiter) {
                if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
                    return false;
                }
            }
            return true;
        }

    }  // namespace frontend
}  // namespace rp