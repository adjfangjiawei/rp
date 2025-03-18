#pragma once

#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/MainLexer/Scanner.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class Lexer {
          public:
            // 默认构造函数，用于测试
            Lexer();
            // 带诊断引擎的构造函数
            explicit Lexer(DiagnosticEngine* diagEngine);

            // 设置源代码
            void setSource(const char* src, size_t length, const std::string& filename);

            // 获取下一个token
            Token nextToken();

            // 预览下一个token而不消费它
            Token peekToken();

            // 预览第n个token而不消费它
            Token peekToken(size_t n);

            // 回退一个token
            void ungetToken(const Token& token);

            // 获取当前位置的行列信息
            std::pair<size_t, size_t> getCurrentPosition() const;

            // 获取错误上下文
            std::string getErrorContext(size_t line, size_t column, size_t context_lines = 2) const;

          protected:
            // 源代码信息
            const char* source;
            size_t sourceLength;
            std::string filename;

            // 当前位置信息
            size_t currentPos;
            size_t currentLine;
            size_t currentColumn;

            // token起始位置信息
            size_t tokenStart;
            size_t tokenLine;
            size_t tokenColumn;

            // Token缓存
            std::deque<Token> tokenCache;
            static const size_t MAX_LOOKAHEAD = 3;

          private:
            // 组件
            std::unique_ptr<Scanner> scanner;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 辅助函数
            void skipWhitespaceAndComments();
            Token createToken(TokenKind kind, const std::string& text = "", bool consumeToken = true);
            void saveTokenStart();
            void restoreToTokenStart();
            void updatePositionFromScanner();

            // 错误处理
            void reportError(const std::string& message, size_t line, size_t column);
            void recoverFromError();

            // 缓存管理
            Token getNextTokenFromSource();
            void fillTokenCache(size_t n);
        };

    }  // namespace frontend
}  // namespace rp
