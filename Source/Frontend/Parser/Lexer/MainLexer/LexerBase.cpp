
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        Lexer::Lexer() : Lexer(std::make_shared<DiagnosticEngine>()) {}

        Lexer::Lexer(std::shared_ptr<DiagnosticEngine> diagEngine) : diagnostics(std::move(diagEngine)) {
            if (!diagnostics) {
                throw std::runtime_error("DiagnosticEngine cannot be null");
            }
            scanner = std::make_unique<Scanner>(diagnostics);
        }

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

        std::string Lexer::getErrorContext(size_t line, size_t column, size_t context_lines) const {
            std::stringstream result;
            size_t start_line = (line > context_lines) ? line - context_lines : 1;
            size_t end_line = line + context_lines;
            size_t current_line = 1;
            size_t pos = 0;

            // 构建错误上下文
            while (pos < sourceLength && current_line <= end_line) {
                if (current_line >= start_line) {
                    // 添加行号
                    result << std::setw(4) << current_line << " | ";

                    // 添加该行内容
                    size_t lineStart = pos;
                    while (pos < sourceLength && source[pos] != '\n') {
                        result << source[pos];
                        pos++;
                    }
                    result << '\n';

                    // 如果是错误所在行，添加错误指示符
                    if (current_line == line) {
                        result << "     | ";
                        for (size_t i = 1; i < column; ++i) {
                            result << ' ';
                        }
                        result << "^\n";
                        result << "     | ";
                        for (size_t i = 1; i < column; ++i) {
                            result << ' ';
                        }
                        result << "Error occurs here\n";
                    }
                }

                // 移动到下一行
                if (pos < sourceLength && source[pos] == '\n') {
                    pos++;
                }
                current_line++;
            }

            return result.str();
        }

        void Lexer::reportError(const std::string& message, size_t line, size_t column) {
            std::stringstream ss;
            ss << message << "\n\n";
            ss << getErrorContext(line, column);

            SourceLocation loc{filename, static_cast<unsigned>(line), static_cast<unsigned>(column)};
            diagnostics->report(DiagnosticLevel::Error, loc, ss.str());
        }

        void Lexer::reportWarning(const std::string& message, size_t line, size_t column) {
            std::stringstream ss;
            ss << message << "\n\n";
            ss << getErrorContext(line, column);

            SourceLocation loc{filename, static_cast<unsigned>(line), static_cast<unsigned>(column)};
            diagnostics->report(DiagnosticLevel::Warning, loc, ss.str());
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
