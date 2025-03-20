
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Frontend/Parser/Lexer/Token/Token.h"
#include "IdentifierScanner.h"
#include "OperatorScanner.h"

namespace rp {
    namespace frontend {

        class Scanner {
          public:
            explicit Scanner(std::shared_ptr<DiagnosticEngine> diagEngine);

            // 源代码管理
            void setSource(const char* src, size_t length, const std::string& filename);

            // 主要扫描函数
            Token scanIdentifier();
            Token scanOperatorOrPunctuation();
            Token scanNumber();
            Token scanString();
            Token scanCharacter();
            Token scanComment();
            Token scanPreprocessor();

            // 位置管理
            size_t getCurrentPos() const { return identifierScanner.getCurrentPos(); }
            size_t getCurrentLine() const { return identifierScanner.getCurrentLine(); }
            size_t getCurrentColumn() const { return identifierScanner.getCurrentColumn(); }
            void setPosition(size_t pos, size_t line, size_t column);

            // 字符判断函数
            bool isIdentifierStart(char c) const { return identifierScanner.isIdentifierStart(c); }
            bool isIdentifierContinue(char c) const { return identifierScanner.isIdentifierContinue(c); }
            bool isDigit(char c) const { return c >= '0' && c <= '9'; }
            bool isHexDigit(char c) const;
            bool isOctalDigit(char c) const { return c >= '0' && c <= '7'; }
            bool isWhitespace(char c) const;

            // 辅助函数
            void skipWhitespace();
            void skipUntilNewline();
            bool lookAhead(const std::string& str) const;
            std::optional<char> peekChar(size_t offset = 1) const;

            // 错误处理
            void reportError(const std::string& message);
            void reportWarning(const std::string& message);
            void skipInvalidToken();

          private:
            IdentifierScanner identifierScanner;
            OperatorScanner operatorScanner;
            const char* source{nullptr};
            size_t sourceLength{0};
            std::string filename;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 位置同步
            void syncScannerPositions();
            void updatePosition(size_t newPos);
            std::pair<size_t, size_t> calculateLineAndColumn(size_t fromPos, size_t toPos) const;

            // 辅助函数
            bool isValidPosition(size_t pos) const { return pos < sourceLength; }
            char getCurrentChar() const { return isValidPosition(getCurrentPos()) ? source[getCurrentPos()] : '\0'; }
            bool isAtEnd() const { return getCurrentPos() >= sourceLength; }

            // 禁用拷贝和赋值
            Scanner(const Scanner&) = delete;
            Scanner& operator=(const Scanner&) = delete;
        };

    }  // namespace frontend
}  // namespace rp
