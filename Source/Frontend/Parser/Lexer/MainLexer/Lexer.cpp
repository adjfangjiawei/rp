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
                    NumberLiteralLexer numberLexer(diagnostics.get());
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
                    CharacterLiteralLexer charLexer(diagnostics.get());
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
                    StringLiteralLexer stringLexer(diagnostics.get());
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
            // 简单的错误恢复：跳过直到找到下一个有效的token开始字符
            while (currentPos < sourceLength) {
                char c = source[currentPos];
                if (isspace(c)) {
                    if (c == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                    currentPos++;
                    continue;
                }

                // 检查是否是可能的token开始
                if (scanner->isIdentifierStart(c) || isdigit(c) || c == '"' || c == '\'' || c == '_' ||
                    strchr("+-*/%<>=!&|^~.,:;()[]{}#", c)) {
                    break;
                }

                currentPos++;
                currentColumn++;
            }
        }

    }  // namespace frontend
}  // namespace rp
