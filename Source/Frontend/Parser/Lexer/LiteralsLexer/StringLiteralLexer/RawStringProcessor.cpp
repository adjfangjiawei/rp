#include "RawStringProcessor.h"

#include "Frontend/Diagnostic/Diagnostic.h"
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
            size_t lineCount = 0;
            const size_t MAX_LINES = 1000;  // 限制最大行数

            // 收集分隔符（允许空分隔符）
            size_t maxDelimiterLength = 16;
            bool foundDelimiter = false;

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
                foundDelimiter = true;

                if (delimiter.length() > maxDelimiterLength) {
                    Token token(TokenKind::Invalid);
                    token.setError("原始字符串分隔符过长（最大长度为" + std::to_string(maxDelimiterLength) + "）",
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
            size_t contentStartLine = startLoc.line;
            size_t contentStartColumn = startLoc.column;

            while (currentPos < source.length()) {
                char c = source[currentPos];

                // 处理换行
                if (c == '\n') {
                    lineCount++;
                    if (lineCount > MAX_LINES) {
                        Token token(TokenKind::Invalid);
                        token.setError("原始字符串超过最大行数限制（" + std::to_string(MAX_LINES) + "行）",
                                       static_cast<unsigned int>(contentStartLine),
                                       static_cast<unsigned int>(contentStartColumn));
                        return token;
                    }
                    content += c;
                    currentPos++;
                    continue;
                }

                // 检查结束序列
                if (c == ')') {
                    size_t tempPos = currentPos;
                    bool isClosing = true;
                    size_t remainingLength = source.length() - tempPos;

                    // 确保有足够的字符来匹配结束序列
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
                                    // 如果结束序列后紧跟着一个有效的分隔符字符，这可能不是真正的结束
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

                    // 如果不是结束序列，继续收集内容
                    content += c;
                    currentPos++;
                    continue;
                }

                // 处理UTF-8字符
                unsigned char uc = static_cast<unsigned char>(c);
                if (uc >= 0x80) {
                    // 使用UnicodeEncoding类处理UTF-8字符
                    size_t bytesRead;
                    auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);

                    if (length == 0) {
                        Token token(TokenKind::Invalid);
                        token.setError("无效的UTF-8序列",
                                       static_cast<unsigned int>(contentStartLine),
                                       static_cast<unsigned int>(contentStartColumn));
                        return token;
                    }

                    // 添加整个UTF-8序列
                    content.append(source.substr(currentPos, length));
                    currentPos += length;
                    continue;
                }

                content += c;
                currentPos++;
            }

            if (!foundClosingSequence) {
                Token token(TokenKind::Invalid);
                token.setError("原始字符串未找到匹配的结束序列 )" + delimiter + "\"",
                               static_cast<unsigned int>(startLoc.line),
                               static_cast<unsigned int>(startLoc.column));
                return token;
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