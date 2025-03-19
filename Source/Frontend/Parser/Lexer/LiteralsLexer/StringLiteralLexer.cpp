#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"

#include <sstream>

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"
#include "StringLiteralLexer/NormalStringProcessor.h"
#include "StringLiteralLexer/RawStringProcessor.h"

namespace rp {
    namespace frontend {

        StringPrefix StringLiteralLexer::parsePrefix() {
            if (currentPos >= sourceLength) {
                return StringPrefix::None;
            }

            std::string input = source.substr(currentPos, std::min(size_t(4), sourceLength - currentPos));
            auto [prefix, length] = StringLiteralUtils::parseStringPrefix(input);

            // 更新位置
            currentPos += length;
            currentColumn += length;

            return prefix;
        }

        Token StringLiteralLexer::createStringToken(const std::string& content, StringPrefix prefix) {
            // 检查数值范围以确保安全转换
            if (currentLine > std::numeric_limits<unsigned int>::max() ||
                currentColumn > std::numeric_limits<unsigned int>::max()) {
                Token errorToken(TokenKind::Invalid);
                errorToken.setError("行号或列号超出有效范围", 0, 0);
                return errorToken;
            }

            Token token(TokenKind::StringLiteral,
                        static_cast<unsigned>(currentLine),
                        static_cast<unsigned>(currentColumn),
                        filename);
            token.setText(content);

            // 设置字符串编码信息
            bool isWide = false;
            bool isUTF8 = false;
            bool isUTF16 = false;
            bool isUTF32 = false;

            switch (prefix) {
                case StringPrefix::L:
                    isWide = true;
                    break;
                case StringPrefix::u8:
                    isUTF8 = true;
                    break;
                case StringPrefix::u:
                    isUTF16 = true;
                    break;
                case StringPrefix::U:
                    isUTF32 = true;
                    break;
                default:
                    break;
            }

            token.setStringEncoding(isWide, isUTF8, isUTF16, isUTF32);
            return token;
        }

        void StringLiteralLexer::reportError(const std::string& message) {
            diagnostics->report(
                DiagnosticLevel::Error,
                SourceLocation{
                    filename, static_cast<unsigned int>(currentLine), static_cast<unsigned int>(currentColumn)},
                message);
        }

        Token StringLiteralLexer::scan() {
            if (currentPos >= sourceLength) {
                Token token(TokenKind::Invalid);
                token.setError("无效的字符串字面量扫描源",
                               static_cast<unsigned int>(currentLine),
                               static_cast<unsigned int>(currentColumn));
                return token;
            }

            // 保存起始位置信息
            size_t startLine = currentLine;
            size_t startColumn = currentColumn;
            SourceLocation startLoc{
                filename, static_cast<unsigned int>(startLine), static_cast<unsigned int>(startColumn)};

            // 解析前缀
            StringPrefix prefix = parsePrefix();
            bool isRawString = StringLiteralUtils::isRawStringPrefix(prefix);

            // 根据字符串类型选择相应的处理器
            Token resultToken;
            if (isRawString) {
                RawStringProcessor processor;
                resultToken = processor.processRawStringLiteral(source, currentPos, startLoc);
            } else {
                NormalStringProcessor processor;
                resultToken = processor.processNormalStringLiteral(source, currentPos, startLoc);
            }

            // 如果处理成功，设置字符串编码信息
            if (resultToken.getKind() == TokenKind::StringLiteral) {
                return createStringToken(std::string(resultToken.getText()), prefix);
            }

            // 如果处理失败，返回错误token
            return resultToken;
        }

    }  // namespace frontend
}  // namespace rp
