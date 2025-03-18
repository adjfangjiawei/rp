
#pragma once

#include <memory>
#include <string>

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
        };

    }  // namespace frontend
}  // namespace rp
