
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"

namespace rp {
    namespace frontend {

        void Lexer::setSource(const char* src, size_t length, const std::string& filename) {
            if (!src && length > 0) {
                throw std::invalid_argument("Source pointer cannot be null when length > 0");
            }

            // 创建源代码的副本
            sourceBuffer = std::string(src, length);
            source = sourceBuffer.c_str();
            sourceLength = length;
            this->filename = filename;

            // 重置状态
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
            tokenStart = 0;
            tokenLine = 1;
            tokenColumn = 1;
            tokenCache.clear();

            // 设置扫描器
            scanner->setSource(source, length, filename);
        }

        Token Lexer::nextToken() {
            if (!tokenCache.empty()) {
                Token token = tokenCache.front();
                tokenCache.pop_front();
                return token;
            }
            return getNextTokenFromSource();
        }

        Token Lexer::peekToken() { return peekToken(1); }

        Token Lexer::peekToken(size_t n) {
            if (n == 0) {
                throw std::invalid_argument("Peek index must be greater than 0");
            }

            fillTokenCache(n);
            if (n <= tokenCache.size()) {
                return tokenCache[n - 1];
            }
            return Token(TokenKind::EndOfFile);
        }

        void Lexer::ungetToken(const Token& token) {
            if (tokenCache.size() >= MAX_CACHE_SIZE) {
                tokenCache.pop_back();
            }
            tokenCache.push_front(token);
        }

        std::pair<size_t, size_t> Lexer::getCurrentPosition() const { return {currentLine, currentColumn}; }

        void Lexer::skipWhitespaceAndComments() {
            while (!isAtEnd()) {
                char c = getCurrentChar();
                char next = peekChar();

                // 处理空白字符
                if (std::isspace(c)) {
                    if (c == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                    currentPos++;
                    continue;
                }

                // 处理注释
                if (c == '/' && next == '/') {
                    skipSingleLineComment();
                    continue;
                }

                if (c == '/' && next == '*') {
                    if (!skipMultiLineComment()) {
                        reportError("Unterminated multi-line comment", currentLine, currentColumn);
                    }
                    continue;
                }

                break;
            }
        }

        void Lexer::skipSingleLineComment() {
            currentPos += 2;
            currentColumn += 2;
            while (!isAtEnd() && getCurrentChar() != '\n') {
                currentPos++;
                currentColumn++;
            }
        }

        bool Lexer::skipMultiLineComment() {
            currentPos += 2;
            currentColumn += 2;
            bool foundEnd = false;

            while (!isAtEnd()) {
                char c = getCurrentChar();
                char next = peekChar();

                if (c == '*' && next == '/') {
                    currentPos += 2;
                    currentColumn += 2;
                    foundEnd = true;
                    break;
                }

                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
                currentPos++;
            }

            return foundEnd;
        }

        std::string Lexer::getErrorContext(size_t line, size_t column, size_t context_lines) const {
            std::string result;
            size_t start_line = (line > context_lines) ? line - context_lines : 1;
            size_t end_line = line + context_lines;
            size_t current_line = 1;
            size_t pos = 0;

            // 构建错误上下文
            while (pos < sourceLength && current_line <= end_line) {
                if (current_line >= start_line) {
                    // 添加行号
                    result += std::to_string(current_line) + " | ";

                    // 添加该行内容
                    size_t lineStart = pos;
                    while (pos < sourceLength && source[pos] != '\n') {
                        pos++;
                    }
                    result += std::string(source + lineStart, pos - lineStart);
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
                if (pos < sourceLength && source[pos] == '\n') {
                    pos++;
                }
                current_line++;
            }

            return result;
        }

        Token Lexer::getNextTokenFromSource() {
            skipWhitespaceAndComments();
            saveTokenStart();

            if (isAtEnd()) {
                return createToken(TokenKind::EndOfFile);
            }

            try {
                // 尝试处理各种类型的token
                if (auto stringToken = handleStringLiteral()) {
                    return *stringToken;
                }
                if (auto charToken = handleCharacterLiteral()) {
                    return *charToken;
                }
                if (auto numberToken = handleNumberLiteral()) {
                    return *numberToken;
                }

                // 标识符或关键字
                if (scanner->isIdentifierStart(getCurrentChar())) {
                    Token token = scanner->scanIdentifier();
                    updatePositionFromScanner();
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

        std::optional<Token> Lexer::handleStringLiteral() {
            char c = getCurrentChar();
            if (c == '"' || (c == 'R' && peekChar() == '"') ||
                ((c == 'L' || c == 'u' || c == 'U') &&
                 (peekChar() == '"' || (peekChar() == 'R' && peekChar(2) == '"')))) {
                StringLiteralLexer stringLexer(diagnostics);
                stringLexer.setSource(source, sourceLength, filename);
                stringLexer.setPosition(currentPos, currentLine, currentColumn);

                auto result = stringLexer.scan();
                if (result.success) {
                    currentPos = stringLexer.getCurrentPos();
                    currentLine = stringLexer.getCurrentLine();
                    currentColumn = stringLexer.getCurrentColumn();
                    return result.token;
                } else {
                    reportError(result.error, currentLine, currentColumn);
                    recoverFromError();
                    return Token(TokenKind::Invalid);
                }
            }
            return std::nullopt;
        }

        std::optional<Token> Lexer::handleCharacterLiteral() {
            if (getCurrentChar() == '\'') {
                CharacterLiteralLexer charLexer(diagnostics);
                charLexer.setSource(source, sourceLength, filename);
                charLexer.setPosition(currentPos, currentLine, currentColumn);

                Token token = charLexer.scan();
                currentPos = charLexer.getCurrentPos();
                currentLine = charLexer.getCurrentLine();
                currentColumn = charLexer.getCurrentColumn();
                return token;
            }
            return std::nullopt;
        }

        std::optional<Token> Lexer::handleNumberLiteral() {
            char c = getCurrentChar();
            if (std::isdigit(c) || (c == '.' && std::isdigit(peekChar()))) {
                NumberLiteralLexer numberLexer(diagnostics);
                numberLexer.setSource(source, sourceLength, filename);
                numberLexer.setPosition(currentPos, currentLine, currentColumn);

                Token token = numberLexer.scan();
                currentPos = numberLexer.getCurrentPos();
                currentLine = numberLexer.getCurrentLine();
                currentColumn = numberLexer.getCurrentColumn();
                return token;
            }
            return std::nullopt;
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
            SourceLocation loc{filename, static_cast<unsigned>(line), static_cast<unsigned>(column)};
            diagnostics->report(DiagnosticLevel::Error, loc, message + "\n" + errorContext);
        }

        void Lexer::reportWarning(const std::string& message, size_t line, size_t column) {
            std::string errorContext = getErrorContext(line, column);
            SourceLocation loc{filename, static_cast<unsigned>(line), static_cast<unsigned>(column)};
            diagnostics->report(DiagnosticLevel::Warning, loc, message + "\n" + errorContext);
        }

        void Lexer::recoverFromError() { skipUntilNextToken(); }

        void Lexer::skipUntilNextToken() {
            while (!isAtEnd()) {
                if (isValidTokenStart(getCurrentChar())) {
                    break;
                }
                if (getCurrentChar() == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
                currentPos++;
            }
        }

        bool Lexer::isValidTokenStart(char c) const {
            return std::isalpha(c) || c == '_' || std::isdigit(c) || c == '"' || c == '\'' || c == '#' ||
                   strchr("+-*/%<>=!&|^~.,:;()[]{}\\", c);
        }

        char Lexer::peekChar(size_t offset) const {
            size_t pos = currentPos + offset;
            return (pos < sourceLength) ? source[pos] : '\0';
        }

        bool Lexer::matchString(const std::string& str) const {
            if (currentPos + str.length() > sourceLength) {
                return false;
            }
            return std::strncmp(source + currentPos, str.c_str(), str.length()) == 0;
        }

        void Lexer::saveTokenStart() {
            tokenStart = currentPos;
            tokenLine = currentLine;
            tokenColumn = currentColumn;
        }

        void Lexer::restoreToTokenStart() {
            currentPos = tokenStart;
            currentLine = tokenLine;
            currentColumn = tokenColumn;
        }

        void Lexer::updatePositionFromScanner() {
            currentPos = scanner->getCurrentPos();
            currentLine = scanner->getCurrentLine();
            currentColumn = scanner->getCurrentColumn();
        }

        Token Lexer::createToken(TokenKind kind, const std::string& text, bool consumeToken) {
            Token token(kind);
            token.filename = filename;
            token.line = tokenLine;
            token.column = tokenColumn;

            if (!text.empty()) {
                token.setText(text);
            } else if (tokenStart < sourceLength) {
                size_t length = currentPos - tokenStart;
                if (length > 0) {
                    token.setText(std::string_view(source + tokenStart, length));
                }
            }

            if (!consumeToken) {
                restoreToTokenStart();
            }

            return token;
        }

    }  // namespace frontend
}  // namespace rp
