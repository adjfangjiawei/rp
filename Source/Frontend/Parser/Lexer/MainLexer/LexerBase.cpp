
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        Lexer::Lexer()
            : source(nullptr),
              currentPos(0),
              currentLine(1),
              currentColumn(1),
              sourceLength(0),
              tokenStart(0),
              tokenLine(1),
              tokenColumn(1) {
            diagnostics = std::make_shared<DiagnosticEngine>();
            scanner = std::make_unique<Scanner>(diagnostics.get());
            KeywordManager::initialize();
        }

        Lexer::Lexer(DiagnosticEngine* diagEngine)
            : source(nullptr),
              currentPos(0),
              currentLine(1),
              currentColumn(1),
              sourceLength(0),
              tokenStart(0),
              tokenLine(1),
              tokenColumn(1) {
            diagnostics = std::shared_ptr<DiagnosticEngine>(diagEngine);
            scanner = std::make_unique<Scanner>(diagEngine);
            KeywordManager::initialize();
        }

        void Lexer::setSource(const char* src, size_t length, const std::string& filename) {
            source = src;
            sourceLength = length;
            this->filename = filename;
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
            tokenStart = 0;
            tokenLine = 1;
            tokenColumn = 1;

            scanner->setSource(src, length, filename);
        }

        Token Lexer::createToken(TokenKind kind, const std::string& text, bool consumeToken) {
            Token token;
            token.kind = kind;
            token.text = text;
            token.filename = filename;
            token.line = tokenLine;
            token.column = tokenColumn;
            if (text.empty()) {
                token.text = std::string_view(source + tokenStart, currentPos - tokenStart);
            }
            if (!consumeToken) {
                restoreToTokenStart();
            }
            return token;
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

        Token Lexer::nextToken() {
            if (!tokenCache.empty()) {
                Token token = std::move(tokenCache.front());
                tokenCache.pop_front();
                return std::move(token);
            }
            return getNextTokenFromSource();
        }

        Token Lexer::peekToken() { return peekToken(1); }

        Token Lexer::peekToken(size_t n) {
            if (n == 0) {
                return Token();
            }

            fillTokenCache(n);
            if (n <= tokenCache.size()) {
                return Token(std::move(tokenCache[n - 1]));
            }
            return Token(TokenKind::EndOfFile);
        }

        void Lexer::ungetToken(const Token& token) { tokenCache.push_front(token); }

        std::pair<size_t, size_t> Lexer::getCurrentPosition() const {
            return std::make_pair(currentLine, currentColumn);
        }

    }  // namespace frontend
}  // namespace rp
