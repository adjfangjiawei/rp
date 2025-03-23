#ifndef RP_STRING_LITERAL_LEXER_H
#define RP_STRING_LITERAL_LEXER_H

#include <memory>
#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "StringLiteralLexer/DiagnosticsHandler.h"
#include "StringLiteralLexer/PrefixProcessor.h"
#include "StringLiteralLexer/StringLiteralUtils.h"
#include "StringLiteralLexer/StringValidator.h"
#include "StringLiteralLexer/TokenCreator.h"
#include "StringLiteralLexer/UnicodeProcessor.h"

namespace rp {
    namespace frontend {
        struct StringScanResult {
            Token token;                        // 处理后的Token
            bool success;                       // 是否成功
            std::string error;                  // 错误信息
            size_t errorPosition;               // 错误位置
            bool hasWarnings;                   // 是否有警告
            std::vector<std::string> warnings;  // 警告信息列表
            size_t consumed;                    // 消耗的字符数
        };

        class StringLiteralLexer {
          public:
            explicit StringLiteralLexer(const std::shared_ptr<DiagnosticEngine>& diagnostics);

            void setSource(const std::string& src, size_t length, const std::string& filename);
            StringScanResult scan();
            void setValidationOptions(const StringValidationOptions& options);

            // 位置信息访问器
            size_t getCurrentPos() const { return currentPos; }
            size_t getCurrentLine() const { return currentLine; }
            size_t getCurrentColumn() const { return currentColumn; }
            void setPosition(size_t pos, size_t line, size_t column);

          private:
            // 源代码信息
            std::string source;
            size_t sourceLength;
            std::string filename;

            // 当前位置信息
            size_t currentPos;
            size_t currentLine;
            size_t currentColumn;

            // 组件
            StringValidationOptions validationOptions{};
            std::unique_ptr<DiagnosticsHandler> diagnosticsHandler;
            std::unique_ptr<StringValidator> stringValidator;

            // 处理方法
            StringScanResult processRawString(const std::string& source, size_t startPos, StringPrefix prefix);
            StringScanResult processNormalString(const std::string& source, size_t startPos, StringPrefix prefix);
            StringScanResult processEscapeSequence(const std::string& source, size_t startPos);

            // 位置更新辅助方法
            void updatePositionInfo(size_t consumed);
            void updatePositionForContent(const std::string_view& content);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RP_STRING_LITERAL_LEXER_H
