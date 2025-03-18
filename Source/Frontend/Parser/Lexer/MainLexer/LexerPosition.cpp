
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

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

    }  // namespace frontend
}  // namespace rp
