#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        Lexer::Lexer()
            : currentPos(0),
              currentLine(1),
              currentColumn(1),
              sourceLength(0),
              tokenStart(0),
              tokenLine(1),
              tokenColumn(1) {
            diagnostics = std::make_shared<DiagnosticEngine>();
            scanner = std::make_unique<Scanner>(diagnostics);
            KeywordManager::initialize();
        }

        Lexer::Lexer(std::shared_ptr<DiagnosticEngine> diagEngine)
            : currentPos(0),
              currentLine(1),
              currentColumn(1),
              sourceLength(0),
              tokenStart(0),
              tokenLine(1),
              tokenColumn(1) {
            diagnostics = diagEngine ? diagEngine : std::make_shared<DiagnosticEngine>();
            scanner = std::make_unique<Scanner>(diagnostics);
            KeywordManager::initialize();
        }

        void Lexer::setSource(const char* src, size_t length, const std::string& filename) {
            // 创建源代码的副本
            sourceBuffer = std::string(src, length);
            source = sourceBuffer.c_str();  // 使用string内部的缓冲区
            sourceLength = length;
            this->filename = filename;
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
            tokenStart = 0;
            tokenLine = 1;
            tokenColumn = 1;

            scanner->setSource(source, length, filename);
        }

        Token Lexer::createToken(TokenKind kind, const std::string& text, bool consumeToken) {
            Token token;
            token.kind = kind;
            token.filename = filename;
            token.line = tokenLine;
            token.column = tokenColumn;

            if (!text.empty()) {
                token.setText(text);
            } else {
                // 从源代码中提取文本
                std::string tokenText(source + tokenStart, currentPos - tokenStart);
                token.setText(std::move(tokenText));
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

        std::pair<size_t, size_t> Lexer::getCurrentPosition() const {
            return std::make_pair(currentLine, currentColumn);
        }

    }  // namespace frontend
}  // namespace rp
