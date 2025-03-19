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

            // 如果位置发生了变化，需要重新计算行号和列号
            if (pos < currentPos) {
                // 向后移动，需要重新计算
                currentLine = 1;
                currentColumn = 1;
                for (size_t i = 0; i < pos; ++i) {
                    if (source[i] == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                }
            } else if (pos > currentPos) {
                // 向前移动，继续计算
                for (size_t i = currentPos; i < pos; ++i) {
                    if (source[i] == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                }
            } else {
                // 位置没有变化，直接使用提供的行号和列号
                currentLine = line;
                currentColumn = column;
            }

            currentPos = pos;
        }

        Token BaseScanner::createToken(TokenKind kind, const std::string& text, size_t startPos) {
            Token token;
            token.kind = kind;
            token.filename = filename;

            // 计算token的准确行号和列号
            size_t tokenLine = 1;
            size_t tokenColumn = 1;
            for (size_t i = 0; i < startPos && i < sourceLength; ++i) {
                if (source[i] == '\n') {
                    tokenLine++;
                    tokenColumn = 1;
                } else {
                    tokenColumn++;
                }
            }
            token.line = tokenLine;
            token.column = tokenColumn;

            // 设置token的文本内容
            if (!text.empty()) {
                token.text = text;
            } else if (startPos < sourceLength) {
                // 使用string_view安全地引用源代码片段
                size_t length = currentPos - startPos;
                if (length == 0) {
                    length = 1;
                }
                if (startPos + length > sourceLength) {
                    length = sourceLength - startPos;
                }
                token.setText(std::string_view(source + startPos, length));

                // 检查文本是否包含换行符，如果包含则需要特殊处理
                if (token.text.find('\n') != std::string::npos) {
                    // 对于多行token（如原始字符串），我们保留完整内容
                    // 但确保行号和列号指向起始位置
                }
            }

            return token;
        }

    }  // namespace frontend
}  // namespace rp
