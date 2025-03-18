#include <algorithm>
#include <cctype>
#include <iostream>
#include <print>

#include "Frontend/Parser/Lexer/ErrorRecovery/ErrorRecovery.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
namespace rp {
    namespace frontend {

        Token Lexer::getNextTokenFromSource() {
            skipWhitespaceAndComments();
            saveTokenStart();

            if (isAtEnd()) {
                return createToken(TokenKind::EndOfFile);
            }

            scanner->setPosition(currentPos, currentLine, currentColumn);

            try {
                // 尝试处理各种类型的token
                if (auto stringToken = handleStringLiteral()) {
                    return *stringToken;
                }
                if (auto charToken = handleCharacterLiteral()) {
                    return *charToken;
                }
                if (auto numberToken = handleNumberLiteral()) {
                    return *numberToken;
                }

                // 标识符或关键字
                if (scanner->isIdentifierStart(getCurrentChar())) {
                    Token token = scanner->scanIdentifier();
                    updatePositionFromScanner();
                    return token;
                }

                // 运算符和标点符号
                Token token = scanner->scanOperatorOrPunctuation();
                updatePositionFromScanner();
                return token;

            } catch (const std::exception& e) {
                reportError(e.what(), currentLine, currentColumn);
                recoverFromError();
                return createToken(TokenKind::Invalid);
            }
        }

        std::optional<Token> Lexer::handleStringLiteral() {
            char c = getCurrentChar();

            // 检查是否是字符串字面量的开始
            bool isStringStart =
                c == '"' || (c == 'R' && peekChar() == '"') ||
                ((c == 'L' || c == 'u' || c == 'U') &&
                 (peekChar() == '"' || (peekChar() == 'R' && peekChar(2) == '"'))) ||
                (c == 'u' && peekChar() == '8' && (peekChar(2) == '"' || (peekChar(2) == 'R' && peekChar(3) == '"')));

            if (!isStringStart) {
                return std::nullopt;
            }

            // 初始化字符串词法分析器
            StringLiteralLexer stringLexer(diagnostics);
            stringLexer.setSource(source, sourceLength, filename);
            stringLexer.setPosition(currentPos, currentLine, currentColumn);

            // 设置验证选项
            StringValidationOptions options;
            options.allowControlChars = true;
            options.strictUTF8 = true;
            options.allowMultiline = true;
            options.allowChineseQuotes = true;
            options.allowSmartQuotes = true;
            stringLexer.setValidationOptions(options);

            // 扫描字符串
            auto result = stringLexer.scan();

            if (!result.success) {
                // 处理错误
                if (!result.error.empty()) {
                    reportError(result.error, currentLine, currentColumn);
                }
                // 处理警告
                for (const auto& warning : result.warnings) {
                    reportWarning(warning, currentLine, currentColumn);
                }

                // // 使用错误恢复机制
                // ErrorRecovery::RecoveryState state = ErrorRecovery::initializeRecovery();
                // state.strategy = ErrorRecovery::Strategy::SkipToNextToken;

                // // 保存当前位置
                // size_t originalPos = currentPos;

                // // 尝试恢复
                // if (ErrorRecovery::recover(sourceBuffer, currentPos, state)) {
                //     // 确保至少前进了一个字符
                //     if (currentPos <= originalPos) {
                //         currentPos = originalPos + 1;
                //     }
                //     // 更新位置信息
                //     updatePosition(source + originalPos, currentPos - originalPos);
                // } else {
                //     // 如果恢复失败，至少前进一个字符
                //     currentPos = originalPos + 1;
                //     updatePosition(source + originalPos, 1);
                //     return Token(TokenKind::Invalid);
                // }
            }

            // 更新位置信息
            if (result.token.getLine() != 0 && result.token.getColumn() != 0) {  // 使用行列号判断是否有位置信息
                currentLine = result.token.getLine();
                currentColumn = result.token.getColumn();
                currentPos += result.consumed;
            } else {
                // 如果Token没有位置信息，手动更新
                updatePosition(result.token.getText().data(), result.token.getText().length());
                currentPos += result.consumed;
            }

            return result.token;
        }

        std::optional<Token> Lexer::handleCharacterLiteral() {
            if (getCurrentChar() == '\'') {
                CharacterLiteralLexer charLexer(diagnostics);
                charLexer.setSource(source, sourceLength, filename);
                charLexer.setPosition(currentPos, currentLine, currentColumn);

                Token token = charLexer.scan();
                currentPos = charLexer.getCurrentPos();
                currentLine = charLexer.getCurrentLine();
                currentColumn = charLexer.getCurrentColumn();
                return token;
            }
            return std::nullopt;
        }

        std::optional<Token> Lexer::handleNumberLiteral() {
            char c = getCurrentChar();
            if (std::isdigit(c) || (c == '.' && std::isdigit(peekChar()))) {
                NumberLiteralLexer numberLexer(diagnostics);
                numberLexer.setSource(source, sourceLength, filename);
                numberLexer.setPosition(currentPos, currentLine, currentColumn);

                Token token = numberLexer.scan();
                currentPos = numberLexer.getCurrentPos();
                currentLine = numberLexer.getCurrentLine();
                currentColumn = numberLexer.getCurrentColumn();
                return token;
            }
            return std::nullopt;
        }

        void Lexer::fillTokenCache(size_t n) {
            while (tokenCache.size() < n) {
                Token token = getNextTokenFromSource();
                tokenCache.push_back(token);
                if (token.kind == TokenKind::EndOfFile) {
                    break;
                }
            }
        }

    }  // namespace frontend
}  // namespace rp
