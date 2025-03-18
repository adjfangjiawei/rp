
#include "BaseScanner.h"

namespace rp {
    namespace frontend {

        BaseScanner::BaseScanner(DiagnosticEngine* diagEngine)
            : source(nullptr), sourceLength(0), currentPos(0), currentLine(1), currentColumn(1) {
            diagnostics = std::shared_ptr<DiagnosticEngine>(diagEngine);
        }

        void BaseScanner::setSource(const char* src, size_t length, const std::string& filename) {
            source = src;
            sourceLength = length;
            this->filename = filename;
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
        }

        void BaseScanner::setPosition(size_t pos, size_t line, size_t column) {
            currentPos = pos;
            currentLine = line;
            currentColumn = column;
        }

        Token BaseScanner::createToken(TokenKind kind, const std::string& text, size_t startPos) {
            Token token;
            token.kind = kind;
            token.text = text;
            token.filename = filename;
            token.line = currentLine;
            token.column = currentColumn;
            if (text.empty()) {
                token.text = std::string_view(source + startPos, 1);
            }
            return token;
        }

    }  // namespace frontend
}  // namespace rp
