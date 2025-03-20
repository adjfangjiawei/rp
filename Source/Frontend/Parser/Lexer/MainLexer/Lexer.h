
#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/Scanner.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class Lexer {
          public:
            // 构造函数
            Lexer();
            explicit Lexer(std::shared_ptr<DiagnosticEngine> diagEngine);
            ~Lexer() = default;

            // 源代码管理
            void setSource(const char* src, size_t length, const std::string& filename);

            // Token操作
            Token nextToken();
            Token peekToken();
            Token peekToken(size_t n);
            void ungetToken(const Token& token);

            // 位置信息
            std::pair<size_t, size_t> getCurrentPosition() const;
            std::string getErrorContext(size_t line, size_t column, size_t context_lines = 2) const;

            // 错误处理
            void reportError(const std::string& message, size_t line, size_t column);
            void reportWarning(const std::string& message, size_t line, size_t column);
            std::shared_ptr<DiagnosticEngine> getDiagnostics() const { return diagnostics; }

          protected:
            // 源代码信息
            std::string sourceBuffer;
            const char* source{nullptr};
            size_t sourceLength{0};
            std::string filename;

            // 位置信息
            size_t currentPos{0};
            size_t currentLine{1};
            size_t currentColumn{1};
            size_t tokenStart{0};
            size_t tokenLine{1};
            size_t tokenColumn{1};

            // Token缓存
            std::deque<Token> tokenCache;
            static constexpr size_t MAX_LOOKAHEAD = 3;
            static constexpr size_t MAX_CACHE_SIZE = 16;

            // 组件
            std::unique_ptr<Scanner> scanner;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // Token处理
            Token createToken(TokenKind kind, const std::string& text = "", bool consumeToken = true);
            void saveTokenStart();
            void restoreToTokenStart();
            void updatePositionFromScanner();

          private:
            // 词法分析
            Token getNextTokenFromSource();
            void fillTokenCache(size_t n);

            // 字符处理
            void skipWhitespaceAndComments();
            void skipSingleLineComment();
            bool skipMultiLineComment();
            std::optional<Token> handleStringLiteral();
            std::optional<Token> handleCharacterLiteral();
            std::optional<Token> handleNumberLiteral();

            // 错误恢复
            void recoverFromError();
            void skipUntilNextToken();
            bool isValidTokenStart(char c) const;

            // 辅助函数
            bool isAtEnd() const { return currentPos >= sourceLength; }
            char getCurrentChar() const { return isAtEnd() ? '\0' : source[currentPos]; }
            char peekChar(size_t offset = 1) const;
            bool matchString(const std::string& str) const;
            void updatePosition(const char* text, size_t length);

            // 禁用拷贝和赋值
            Lexer(const Lexer&) = delete;
            Lexer& operator=(const Lexer&) = delete;
        };

    }  // namespace frontend
}  // namespace rp
