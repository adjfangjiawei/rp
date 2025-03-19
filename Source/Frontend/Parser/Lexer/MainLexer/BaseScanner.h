#pragma once

#include <memory>
#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class BaseScanner {
          public:
            BaseScanner(std::shared_ptr<DiagnosticEngine> diagEngine);
            virtual ~BaseScanner() = default;

            // 基础源代码管理
            void setSource(const char* src, size_t length, const std::string& filename);

            // 位置管理
            size_t getCurrentPos() const { return currentPos; }
            size_t getCurrentLine() const { return currentLine; }
            size_t getCurrentColumn() const { return currentColumn; }
            void setPosition(size_t pos, size_t line, size_t column);

          protected:
            const char* source;  // 指向Lexer管理的源代码副本
            size_t sourceLength;
            std::string filename;

            size_t currentPos;
            size_t currentLine;
            size_t currentColumn;

            std::shared_ptr<DiagnosticEngine> diagnostics;

            // Token创建辅助函数
            Token createToken(TokenKind kind, const std::string& text = "", size_t startPos = 0);

            // 安全的字符访问
            char getCurrentChar() const { return (currentPos < sourceLength) ? source[currentPos] : '\0'; }

            char peekChar(size_t offset = 1) const {
                size_t pos = currentPos + offset;
                return (pos < sourceLength) ? source[pos] : '\0';
            }
        };

    }  // namespace frontend
}  // namespace rp
