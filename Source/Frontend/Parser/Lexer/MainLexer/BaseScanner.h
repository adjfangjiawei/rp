
#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class BaseScanner {
          public:
            explicit BaseScanner(std::shared_ptr<DiagnosticEngine> diagEngine);
            virtual ~BaseScanner() = default;

            // 基础源代码管理
            virtual void setSource(const char* src, size_t length, const std::string& filename);

            // 位置管理
            size_t getCurrentPos() const { return currentPos; }
            size_t getCurrentLine() const { return currentLine; }
            size_t getCurrentColumn() const { return currentColumn; }
            virtual void setPosition(size_t pos, size_t line, size_t column);

            // 源码范围检查
            bool isEndOfFile() const { return currentPos >= sourceLength; }
            bool hasMoreChars() const { return currentPos < sourceLength; }
            size_t getRemainingLength() const { return sourceLength - currentPos; }

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
            Token createToken(TokenKind kind, size_t startPos, size_t length);

            // 安全的字符访问
            char getCurrentChar() const { return (currentPos < sourceLength) ? source[currentPos] : '\0'; }

            char peekChar(size_t offset = 1) const {
                size_t pos = currentPos + offset;
                return (pos < sourceLength) ? source[pos] : '\0';
            }

            // 字符处理辅助函数
            bool isAtLineEnd() const;
            void skipLineEnd();
            void skipWhitespace();
            bool matchString(const char* str, size_t length) const;
            bool matchChar(char c) const { return getCurrentChar() == c; }

            // 位置计算辅助函数
            void calculateLineAndColumn(size_t pos, size_t& line, size_t& column) const;
            std::pair<size_t, size_t> getLineAndColumn(size_t pos) const;

            // 错误处理
            void reportError(const std::string& message);
            void reportWarning(const std::string& message);

          private:
            // 禁用拷贝和赋值
            BaseScanner(const BaseScanner&) = delete;
            BaseScanner& operator=(const BaseScanner&) = delete;
        };

    }  // namespace frontend
}  // namespace rp
