#ifndef STRINGLITERALLEXER_H
#define STRINGLITERALLEXER_H

#include <memory>
#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Token/TokenKind.h"
#include "StringLiteralLexer/EscapeSequenceProcessor.h"
#include "StringLiteralLexer/NormalStringProcessor.h"
#include "StringLiteralLexer/RawStringProcessor.h"
#include "StringLiteralLexer/StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        // 字符串扫描结果
        struct StringScanResult {
            Token token;                        // 处理后的Token
            bool success;                       // 是否成功
            std::string error;                  // 错误信息
            size_t errorPosition;               // 错误位置
            bool hasWarnings;                   // 是否有警告
            std::vector<std::string> warnings;  // 警告信息列表
        };

        // 字符串验证选项
        struct StringValidationOptions {
            bool allowControlChars;    // 是否允许控制字符
            bool strictUTF8;           // 是否严格检查UTF-8
            bool allowInvalidUnicode;  // 是否允许无效的Unicode
            bool allowMultiline;       // 是否允许多行字符串
            size_t maxLength;          // 最大字符串长度
            size_t maxLines;           // 最大行数
            bool allowChineseQuotes;   // 是否允许中文引号
            bool allowSmartQuotes;     // 是否允许智能引号
        };

        class StringLiteralLexer {
          public:
            explicit StringLiteralLexer(const std::shared_ptr<DiagnosticEngine>& diagnostics)
                : diagnostics(diagnostics),
                  source(""),
                  sourceLength(0),
                  currentPos(0),
                  currentLine(1),
                  currentColumn(1) {
                initializeValidationOptions();
            }

            // 设置源代码和相关信息
            void setSource(const std::string& src, size_t length, const std::string& filename) {
                source = src;
                sourceLength = length;
                this->filename = filename;
                resetPosition();
            }

            // 扫描字符串字面量
            StringScanResult scan();

            // 设置验证选项
            void setValidationOptions(const StringValidationOptions& options) { validationOptions = options; }

            // 获取当前位置信息
            size_t getCurrentPos() const { return currentPos; }
            size_t getCurrentLine() const { return currentLine; }
            size_t getCurrentColumn() const { return currentColumn; }

            // 设置位置信息
            void setPosition(size_t pos, size_t line, size_t column) {
                currentPos = pos;
                currentLine = line;
                currentColumn = column;
            }

          private:
            // 初始化验证选项
            void initializeValidationOptions() {
                validationOptions.allowControlChars = false;
                validationOptions.strictUTF8 = true;
                validationOptions.allowInvalidUnicode = false;
                validationOptions.allowMultiline = true;
                validationOptions.maxLength = 65536;
                validationOptions.maxLines = 1000;
                validationOptions.allowChineseQuotes = true;
                validationOptions.allowSmartQuotes = true;
            }

            // 重置位置信息
            void resetPosition() {
                currentPos = 0;
                currentLine = 1;
                currentColumn = 1;
            }

            // 解析字符串前缀
            StringPrefix parsePrefix();

            // 创建字符串token
            Token createStringToken(const std::string& content, StringPrefix prefix);

            // 验证字符串内容
            bool validateString(const std::string& content, std::string& error, std::vector<std::string>& warnings);

            // 处理错误和警告
            void handleDiagnostics(const std::string& error,
                                   const std::vector<std::string>& warnings,
                                   const SourceLocation& loc);

            // 创建基本token
            Token createToken(TokenKind kind, const SourceLocation& loc) {
                Token token;
                token.kind = kind;
                token.line = loc.line;
                token.column = loc.column;
                token.filename = loc.filename;
                return token;
            }

            // 更新位置信息
            void updatePosition(char c) {
                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
            }

            // 成员变量
            std::shared_ptr<DiagnosticEngine> diagnostics;
            std::string source;
            size_t sourceLength;
            std::string filename;
            StringValidationOptions validationOptions;

            // 位置信息
            size_t currentPos;
            size_t currentLine;
            size_t currentColumn;
        };

    }  // namespace frontend
}  // namespace rp

#endif  // STRINGLITERALLEXER_H
