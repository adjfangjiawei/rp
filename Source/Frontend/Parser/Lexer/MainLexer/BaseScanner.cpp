#include "BaseScanner.h"

namespace rp {
    namespace frontend {

        BaseScanner::BaseScanner(std::shared_ptr<DiagnosticEngine> diagEngine)
            : source(nullptr),
              sourceLength(0),
              currentPos(0),
              currentLine(1),
              currentColumn(1),
              diagnostics(std::move(diagEngine)) {}

        void BaseScanner::setSource(const char* src, size_t length, const std::string& filename) {
            // 不创建副本，而是使用Lexer管理的源代码副本
            source = src;
            sourceLength = length;
            this->filename = filename;
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
        }

        void BaseScanner::setPosition(size_t pos, size_t line, size_t column) {
            // 添加边界检查
            if (pos > sourceLength) {
                pos = sourceLength;
            }
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

            if (text.empty() && startPos < sourceLength) {
                // 使用string_view安全地引用源代码片段
                size_t length = currentPos - startPos;
                if (length == 0) length = 1;
                if (startPos + length > sourceLength) {
                    length = sourceLength - startPos;
                }
                token.text = std::string_view(source + startPos, length);
            }
            return token;
        }

    }  // namespace frontend
}  // namespace rp
