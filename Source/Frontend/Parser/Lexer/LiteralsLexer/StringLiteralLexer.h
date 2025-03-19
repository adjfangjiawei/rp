
#ifndef STRINGLITERALLEXER_H
#define STRINGLITERALLEXER_H

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Token/TokenKind.h"
#include "StringLiteralLexer/EscapeSequenceProcessor.h"
#include "StringLiteralLexer/NormalStringProcessor.h"
#include "StringLiteralLexer/RawStringProcessor.h"
#include "StringLiteralLexer/StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        class StringLiteralLexer {
          public:
            explicit StringLiteralLexer(const std::shared_ptr<DiagnosticEngine>& diagnostics)
                : diagnostics(diagnostics),
                  source(""),
                  sourceLength(0),
                  currentPos(0),
                  currentLine(1),
                  currentColumn(1) {}

            // 设置源代码和相关信息
            void setSource(const std::string& src, size_t length, const std::string& filename) {
                source = src;
                sourceLength = length;
                this->filename = filename;
            }

            // 扫描字符串字面量
            Token scan();

            // 当前位置信息
            size_t currentPos;
            size_t currentLine;
            size_t currentColumn;

          private:
            // 解析字符串前缀
            StringPrefix parsePrefix();

            // 创建字符串token
            Token createStringToken(const std::string& content, StringPrefix prefix);

            // 处理转义序列
            std::string processEscapes(const std::string& input, bool& success);

            // 报告错误
            void reportError(const std::string& message);

            // 创建基本token
            Token createToken(TokenKind kind, const SourceLocation& loc) {
                Token token;
                token.kind = kind;
                token.line = loc.line;
                token.column = loc.column;
                token.filename = loc.filename;
                return token;
            }

            std::shared_ptr<DiagnosticEngine> diagnostics;
            std::string source;
            size_t sourceLength;
            std::string filename;
        };

    }  // namespace frontend
}  // namespace rp

#endif  // STRINGLITERALLEXER_H
