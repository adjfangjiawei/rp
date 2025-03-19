#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

#include <cstring>

#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"

namespace rp {
    namespace frontend {

        std::string Lexer::getErrorContext(size_t line, size_t column, size_t context_lines) const {
            std::string result;
            size_t start_line = (line > context_lines) ? line - context_lines : 1;
            size_t end_line = line + context_lines;
            size_t current_line = 1;
            size_t pos = 0;

            while (pos < sourceLength && current_line <= end_line) {
                if (current_line >= start_line) {
                    // 添加行号
                    result += std::to_string(current_line) + " | ";

                    // 添加该行内容
                    while (pos < sourceLength && source[pos] != '\n') {
                        result += source[pos++];
                    }
                    result += '\n';

                    // 如果是错误所在行，添加错误指示符
                    if (current_line == line) {
                        result += "  | ";
                        for (size_t i = 1; i < column; ++i) {
                            result += ' ';
                        }
                        result += "^\n";
                    }
                }

                // 移动到下一行
                while (pos < sourceLength && source[pos] != '\n') {
                    pos++;
                }
                if (pos < sourceLength && source[pos] == '\n') {
                    pos++;
                }
                current_line++;
            }

            return result;
        }

        Token Lexer::getNextTokenFromSource() {
            // 跳过空白字符和注释
            skipWhitespaceAndComments();

            // 记录token的起始位置
            saveTokenStart();

            // 到达文件末尾
            if (currentPos >= sourceLength) {
                return createToken(TokenKind::EndOfFile);
            }

            try {
                char c = source[currentPos];

                // 更新扫描器的位置
                scanner->setPosition(currentPos, currentLine, currentColumn);

                // 标识符或关键字
                if (scanner->isIdentifierStart(c)) {
                    Token token = scanner->scanIdentifier();
                    updatePositionFromScanner();
                    return token;
                }

                // 数字
                if (isdigit(c) || (c == '.' && currentPos + 1 < sourceLength && isdigit(source[currentPos + 1]))) {
                    NumberLiteralLexer numberLexer(this->diagnostics);
                    numberLexer.setSource(source, sourceLength, filename);
                    numberLexer.currentPos = currentPos;
                    numberLexer.currentLine = currentLine;
                    numberLexer.currentColumn = currentColumn;
                    Token token = numberLexer.scan();
                    currentPos = numberLexer.currentPos;
                    currentLine = numberLexer.currentLine;
                    currentColumn = numberLexer.currentColumn;
                    return token;
                }

                // 字符字面量
                if (c == '\'') {
                    CharacterLiteralLexer charLexer(this->diagnostics);
                    charLexer.setSource(source, sourceLength, filename);
                    charLexer.currentPos = currentPos;
                    charLexer.currentLine = currentLine;
                    charLexer.currentColumn = currentColumn;
                    Token token = charLexer.scan();
                    currentPos = charLexer.currentPos;
                    currentLine = charLexer.currentLine;
                    currentColumn = charLexer.currentColumn;
                    return token;
                }

                // 字符串字面量
                if (c == '"' || (c == 'L' && currentPos + 1 < sourceLength && source[currentPos + 1] == '"') ||
                    (c == 'u' && currentPos + 1 < sourceLength && source[currentPos + 1] == '"') ||
                    (c == 'U' && currentPos + 1 < sourceLength && source[currentPos + 1] == '"') ||
                    (c == 'u' && currentPos + 1 < sourceLength && source[currentPos + 1] == '8' &&
                     currentPos + 2 < sourceLength && source[currentPos + 2] == '"') ||
                    (c == 'R' && currentPos + 1 < sourceLength && source[currentPos + 1] == '"')) {
                    StringLiteralLexer stringLexer(this->diagnostics);
                    stringLexer.setSource(source, sourceLength, filename);
                    stringLexer.currentPos = currentPos;
                    stringLexer.currentLine = currentLine;
                    stringLexer.currentColumn = currentColumn;
                    Token token = stringLexer.scan();
                    currentPos = stringLexer.currentPos;
                    currentLine = stringLexer.currentLine;
                    currentColumn = stringLexer.currentColumn;
                    return token;
                }

                // 运算符和标点符号
                Token token = scanner->scanOperatorOrPunctuation();
                updatePositionFromScanner();
                return token;

            } catch (const std::exception& e) {
                reportError(e.what(), currentLine, currentColumn);
                recoverFromError();
                return createToken(TokenKind::Invalid);
            }
        }

        void Lexer::fillTokenCache(size_t n) {
            while (tokenCache.size() < n) {
                Token token = getNextTokenFromSource();
                tokenCache.push_back(token);
                if (token.kind == TokenKind::EndOfFile) {
                    break;
                }
            }
        }

        void Lexer::reportError(const std::string& message, size_t line, size_t column) {
            std::string errorContext = getErrorContext(line, column);
            SourceLocation loc;
            loc.filename = filename;
            loc.line = line;
            loc.column = column;
            diagnostics->report(DiagnosticLevel::Error, loc, message + "\n" + errorContext);
        }

        void Lexer::recoverFromError() {
            // 增强的错误恢复：跳过直到找到下一个明确的token边界
            bool inString = false;
            bool inChar = false;
            bool inComment = false;
            bool inRawString = false;
            std::string rawDelimiter;
            size_t startPos = currentPos;

            while (currentPos < sourceLength) {
                char c = source[currentPos];
                char next = (currentPos + 1 < sourceLength) ? source[currentPos + 1] : '\0';
                char prev = (currentPos > 0) ? source[currentPos - 1] : '\0';

                // 处理换行
                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                    // 换行会终止单行注释和未终止的普通字符串/字符字面量
                    // 但不会终止原始字符串
                    inComment = false;
                    if (!inRawString) {
                        inString = false;
                        inChar = false;
                    }
                } else {
                    currentColumn++;
                }

                // 检查注释
                if (!inString && !inChar && !inRawString && c == '/' && next == '/') {
                    inComment = true;
                }

                // 检查原始字符串
                if (!inString && !inChar && !inComment && c == 'R' && next == '"') {
                    inRawString = true;
                    currentPos += 2;  // 跳过R"
                    // 读取分隔符
                    size_t delimStart = currentPos;
                    while (currentPos < sourceLength && source[currentPos] != '(') {
                        if (source[currentPos] == '\n') {
                            inRawString = false;
                            break;
                        }
                        rawDelimiter += source[currentPos];
                        currentPos++;
                    }
                    if (inRawString && currentPos < sourceLength && source[currentPos] == '(') {
                        currentPos++;  // 跳过(
                    } else {
                        inRawString = false;
                        rawDelimiter.clear();
                        currentPos = delimStart;
                    }
                    continue;
                }

                // 检查原始字符串的结束
                if (inRawString && c == ')') {
                    size_t endPos = currentPos + 1;
                    bool foundEnd = true;
                    // 检查分隔符
                    for (char delimChar : rawDelimiter) {
                        if (endPos >= sourceLength || source[endPos] != delimChar) {
                            foundEnd = false;
                            break;
                        }
                        endPos++;
                    }
                    if (foundEnd && endPos < sourceLength && source[endPos] == '"') {
                        currentPos = endPos + 1;  // 跳过结束引号
                        inRawString = false;
                        rawDelimiter.clear();
                        break;  // 找到有效的token边界
                    }
                }

                // 检查普通字符串边界
                if (!inComment && !inChar && !inRawString && c == '"' && prev != '\\') {
                    if (!inString) {
                        // 找到新的字符串开始
                        break;
                    }
                    inString = !inString;
                }

                // 检查字符字面量边界
                if (!inComment && !inString && !inRawString && c == '\'' && prev != '\\') {
                    if (!inChar) {
                        // 找到新的字符字面量开始
                        break;
                    }
                    inChar = !inChar;
                }

                // 如果不在任何字面量或注释中，检查是否是有效的token开始
                if (!inString && !inChar && !inComment && !inRawString) {
                    if (scanner->isIdentifierStart(c) || isdigit(c) || c == '_' ||
                        strchr("+-*/%<>=!&|^~.,:;()[]{}#", c)) {
                        break;
                    }
                }

                currentPos++;
            }

            // 确保至少前进了一个字符
            if (currentPos == startPos) {
                currentPos++;
            }
        }

    }  // namespace frontend
}  // namespace rp
