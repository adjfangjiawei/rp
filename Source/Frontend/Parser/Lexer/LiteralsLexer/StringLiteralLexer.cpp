#include "StringLiteralLexer.h"

#include <iostream>
#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/DiagnosticsHandler.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/EscapeSequenceProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/NormalStringProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/PrefixProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/RawStringProcessor.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/StringLiteralUtils.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/StringValidator.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/TokenCreator.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/UnicodeProcessor.h"
using namespace std::string_literals;

namespace rp {
    namespace frontend {

        StringLiteralLexer::StringLiteralLexer(const std::shared_ptr<DiagnosticEngine>& diagnostics)
            : currentPos(0), currentLine(1), currentColumn(1), sourceLength(0) {
            diagnosticsHandler = std::make_unique<DiagnosticsHandler>(diagnostics);
            stringValidator = std::make_unique<StringValidator>(validationOptions);
        }

        void StringLiteralLexer::setSource(const std::string& src, size_t length, const std::string& fname) {
            source = src;
            sourceLength = length;
            filename = fname;
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
        }

        void StringLiteralLexer::setValidationOptions(const StringValidationOptions& options) {
            validationOptions = options;
            stringValidator = std::make_unique<StringValidator>(options);
        }

        void StringLiteralLexer::setPosition(size_t pos, size_t line, size_t column) {
            currentPos = pos;
            currentLine = line;
            currentColumn = column;
        }

        StringScanResult StringLiteralLexer::scan() {
            StringScanResult result;
            result.success = false;
            result.consumed = 0;

            if (currentPos >= sourceLength) {
                diagnosticsHandler->reportError(
                    "Unexpected end of file",
                    SourceLocation{filename, static_cast<unsigned>(currentLine), static_cast<unsigned>(currentColumn)});
                return result;
            }

            // 使用StringLiteralUtils解析前缀
            auto [prefix, prefixLength] = StringLiteralUtils::parseStringPrefix(source.substr(currentPos));
            std::string prefixStr = StringLiteralUtils::getPrefixString(prefix);

            // 更新前缀的列位置
            for (size_t i = 0; i < prefixLength; ++i) {
                currentColumn++;
            }

            SourceLocation startLoc{filename, static_cast<unsigned>(currentLine), static_cast<unsigned>(currentColumn)};

            // 根据前缀类型选择处理方式
            if (StringLiteralUtils::isRawStringPrefix(prefix)) {
                auto rawResult =
                    RawStringProcessor::processRawStringLiteral(source, currentPos + prefixLength, prefix, startLoc);
                if (!rawResult.success) {
                    diagnosticsHandler->handleDiagnostics(rawResult.error, rawResult.warnings, startLoc);
                    result.consumed = prefixLength + rawResult.consumed;
                    result.token = rawResult.token;
                    // 使用处理结果中的位置信息
                    currentPos += result.consumed;
                    if (rawResult.endPos.line > 0) {  // 确保有有效的位置信息
                        currentLine = rawResult.endPos.line;
                        currentColumn = rawResult.endPos.column;
                    }
                    return result;
                }
                result.token = rawResult.token;
                result.success = true;
                result.consumed = prefixLength + rawResult.consumed;
                // 使用处理结果中的位置信息
                currentLine = rawResult.endPos.line;
                currentColumn = rawResult.endPos.column;
            } else {
                auto normalResult = NormalStringProcessor::processNormalStringLiteral(
                    source, currentPos + prefixLength, prefix, startLoc);
                if (!normalResult.success) {
                    if (!normalResult.token.text.empty()) {
                        normalResult.error +=
                            "解析出来的字符串内容为"s + "\""s + std::string(normalResult.token.text) + "\"";
                    }
                    diagnosticsHandler->handleDiagnostics(normalResult.error, normalResult.warnings, startLoc);
                    result.consumed = prefixLength + normalResult.consumed;
                    result.token = normalResult.token;
                    // 使用处理结果中的位置信息
                    currentPos += result.consumed;
                    if (normalResult.endPos.line > 0) {  // 确保有有效的位置信息
                        currentLine = normalResult.endPos.line;
                        currentColumn = normalResult.endPos.column;
                    }
                    return result;
                }
                result.token = normalResult.token;
                result.success = true;
                result.consumed = prefixLength + normalResult.consumed;
                // 使用处理结果中的位置信息
                currentLine = normalResult.endPos.line;
                currentColumn = normalResult.endPos.column;
            }

            // 验证字符串内容
            std::string validationError;
            std::vector<std::string> validationWarnings;
            if (!stringValidator->validate(std::string(result.token.getText()), validationError, validationWarnings)) {
                diagnosticsHandler->handleDiagnostics(validationError, validationWarnings, startLoc);
                result.success = false;
                return result;
            }

            // 设置Token的编码信息
            result.token = TokenCreator::createStringToken(
                std::string(result.token.getText()), prefix, currentLine, currentColumn, filename);

            // 更新位置信息
            currentPos += result.consumed;

            return result;
        }

    }  // namespace frontend
}  // namespace rp
