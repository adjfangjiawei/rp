
#include <algorithm>
#include <cctype>

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
            if (c == '"' || (c == 'R' && peekChar() == '"') ||
                ((c == 'L' || c == 'u' || c == 'U') &&
                 (peekChar() == '"' || (peekChar() == 'R' && peekChar(2) == '"')))) {
                StringLiteralLexer stringLexer(diagnostics);
                stringLexer.setSource(source, sourceLength, filename);
                stringLexer.setPosition(currentPos, currentLine, currentColumn);

                auto result = stringLexer.scan();
                if (result.success) {
                    currentPos = stringLexer.getCurrentPos();
                    currentLine = stringLexer.getCurrentLine();
                    currentColumn = stringLexer.getCurrentColumn();
                    return result.token;
                } else {
                    reportError(result.error, currentLine, currentColumn);
                    recoverFromError();
                    return Token(TokenKind::Invalid);
                }
            }
            return std::nullopt;
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
