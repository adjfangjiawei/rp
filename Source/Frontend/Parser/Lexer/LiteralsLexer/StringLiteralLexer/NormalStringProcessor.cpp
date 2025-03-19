
#include "NormalStringProcessor.h"

#include "../StringLiteralLexer/EscapeSequenceProcessor.h"
#include "../StringLiteralLexer/StringLiteralUtils.h"
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"

namespace rp {
    namespace frontend {

        Token NormalStringProcessor::processNormalStringLiteral(const std::string& source,
                                                                size_t& currentPos,
                                                                const SourceLocation& startLoc) {
            if (currentPos >= source.length() || source[currentPos] != '"') {
                Token token;
                token.kind = TokenKind::Invalid;
                token.line = startLoc.line;
                token.column = startLoc.column;
                token.setText(std::string("字符串必须以双引号开始"));
                return token;
            }

            currentPos++;  // 跳过开头的引号
            std::string content;
            bool inMultiLine = false;
            size_t lineCount = 0;
            const size_t MAX_LINES = 1000;  // 限制最大行数

            while (currentPos < source.length()) {
                // 处理字符串终止
                if (source[currentPos] == '"' && StringLiteralUtils::isUnescapedQuote(source, currentPos)) {
                    currentPos++;
                    Token token;
                    token.kind = TokenKind::StringLiteral;
                    token.setText(content);
                    token.line = startLoc.line;
                    token.column = startLoc.column;
                    return token;
                }

                // 处理转义序列
                if (source[currentPos] == '\\') {
                    if (currentPos + 1 >= source.length()) {
                        Token token;
                        token.kind = TokenKind::Invalid;
                        token.line = startLoc.line;
                        token.column = startLoc.column;
                        token.setText(std::string("不完整的转义序列"));
                        return token;
                    }

                    char next = source[currentPos + 1];

                    // 处理行继续符
                    if (next == '\n' || next == '\r') {
                        inMultiLine = true;
                        if (next == '\r' && currentPos + 2 < source.length() && source[currentPos + 2] == '\n') {
                            currentPos += 3;  // 跳过 \r\n
                        } else {
                            currentPos += 2;  // 跳过 \n 或 \r
                        }
                        lineCount++;
                        if (lineCount > MAX_LINES) {
                            Token token;
                            token.kind = TokenKind::Invalid;
                            token.line = startLoc.line;
                            token.column = startLoc.column;
                            token.setText(std::string("字符串超过最大行数限制"));
                            return token;
                        }

                        // 跳过下一行开头的空白字符
                        while (currentPos < source.length() && StringLiteralUtils::isWhitespace(source[currentPos])) {
                            currentPos++;
                        }
                        continue;
                    }

                    // 处理转义序列
                    std::string escapeError;
                    size_t escapePos = currentPos;
                    std::string processed =
                        EscapeSequenceProcessor::processEscapeSequence(source, escapePos, escapeError);
                    if (!escapeError.empty()) {
                        Token token;
                        token.kind = TokenKind::Invalid;
                        token.line = startLoc.line;
                        token.column = startLoc.column;
                        token.setText(escapeError);
                        return token;
                    }
                    content += processed;
                    currentPos = escapePos;
                    continue;
                }

                // 处理换行符
                if (source[currentPos] == '\n' || source[currentPos] == '\r') {
                    if (!inMultiLine) {
                        Token token;
                        token.kind = TokenKind::Invalid;
                        token.line = startLoc.line;
                        token.column = startLoc.column;
                        token.setText(std::string("未终止的字符串字面量"));
                        return token;
                    }

                    lineCount++;
                    if (lineCount > MAX_LINES) {
                        Token token;
                        token.kind = TokenKind::Invalid;
                        token.line = startLoc.line;
                        token.column = startLoc.column;
                        token.setText(std::string("字符串超过最大行数限制"));
                        return token;
                    }

                    // 统一处理换行符
                    if (source[currentPos] == '\r' && currentPos + 1 < source.length() &&
                        source[currentPos + 1] == '\n') {
                        content += '\n';  // 将 CRLF 转换为 LF
                        currentPos += 2;
                    } else {
                        content += '\n';  // 将单个 CR 或 LF 转换为 LF
                        currentPos++;
                    }

                    // 跳过下一行开头的空白字符
                    while (currentPos < source.length() && StringLiteralUtils::isWhitespace(source[currentPos])) {
                        currentPos++;
                    }
                    continue;
                }

                // 处理 UTF-8 字符
                unsigned char currentChar = static_cast<unsigned char>(source[currentPos]);
                if (currentChar >= 0x80) {
                    auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);
                    if (length == 0) {
                        Token token;
                        token.kind = TokenKind::Invalid;
                        token.line = startLoc.line;
                        token.column = startLoc.column;
                        token.setText(std::string("无效的UTF-8序列"));
                        return token;
                    }

                    content.append(source.substr(currentPos, length));
                    currentPos += length;
                    continue;
                }

                // 处理普通ASCII字符
                if (!StringLiteralUtils::isValidStringChar(currentChar)) {
                    Token token;
                    token.kind = TokenKind::Invalid;
                    token.line = startLoc.line;
                    token.column = startLoc.column;
                    token.setText(std::string("字符串中包含无效字符"));
                    return token;
                }
                content += source[currentPos];
                currentPos++;
            }

            Token token;
            token.kind = TokenKind::Invalid;
            token.line = startLoc.line;
            token.column = startLoc.column;
            token.setText(std::string("未终止的字符串字面量"));
            return token;
        }

        bool NormalStringProcessor::validateStringContent(const std::string& str, std::string& error) {
            if (str.empty()) {
                return true;  // 允许空字符串
            }

            size_t i = 0;
            while (i < str.length()) {
                unsigned char c = static_cast<unsigned char>(str[i]);

                // 处理UTF-8字符
                if (c >= 0x80) {
                    auto [valid, bytesRead] = StringLiteralUtils::validateUTF8Sequence(str, i);
                    if (!valid) {
                        error = "无效的UTF-8序列";
                        return false;
                    }
                    i += bytesRead;
                    continue;
                }

                // 检查ASCII字符的有效性
                if (c < 0x20 && c != '\n' && c != '\r' && c != '\t') {
                    error = "控制字符必须通过转义序列表示";
                    return false;
                }

                if (c == 0x7F) {  // DEL字符
                    error = "无效的控制字符(DEL)";
                    return false;
                }

                if (!StringLiteralUtils::isValidStringChar(c)) {
                    error = "字符串中包含无效字符";
                    return false;
                }

                i++;
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
