
#include "OperatorScanner.h"

namespace rp {
    namespace frontend {

        Token OperatorScanner::scanOperatorOrPunctuation() {
            char c = source[currentPos];
            currentPos++;
            currentColumn++;

            // 先处理单字符运算符和标点符号
            Token singleCharResult = handleSingleCharOperator(c);
            if (singleCharResult.kind != TokenKind::Invalid) {
                return singleCharResult;
            }

            // 处理双字符和三字符运算符
            if (currentPos < sourceLength) {
                char next = source[currentPos];
                Token doubleCharResult = handleDoubleCharOperator(c, next);
                if (doubleCharResult.kind != TokenKind::Invalid) {
                    return doubleCharResult;
                }

                // 处理三字符运算符
                if (currentPos + 1 < sourceLength) {
                    char third = source[currentPos + 1];
                    Token tripleCharResult = handleTripleCharOperator(c, next, third);
                    if (tripleCharResult.kind != TokenKind::Invalid) {
                        return tripleCharResult;
                    }
                }
            }

            // 如果没有匹配到任何有效的运算符，返回无效token
            diagnostics->report(
                DiagnosticLevel::Error,
                {filename, static_cast<unsigned int>(currentLine), static_cast<unsigned int>(currentColumn)},
                "Invalid operator or punctuation");
            return createToken(TokenKind::Invalid);
        }

        Token OperatorScanner::handleSingleCharOperator(char c) {
            switch (c) {
                case '(':
                    return createToken(TokenKind::LParen);
                case ')':
                    return createToken(TokenKind::RParen);
                case '[':
                    return createToken(TokenKind::LSquare);
                case ']':
                    return createToken(TokenKind::RSquare);
                case '{':
                    return createToken(TokenKind::LBrace);
                case '}':
                    return createToken(TokenKind::RBrace);
                case ';':
                    return createToken(TokenKind::Semicolon);
                case ',':
                    return createToken(TokenKind::Comma);
                case '~':
                    return createToken(TokenKind::Tilde);
                case '?':
                    return createToken(TokenKind::Question);
                case '*':
                    return createToken(TokenKind::Star);
                case '/':
                    return createToken(TokenKind::Slash);
                case '%':
                    return createToken(TokenKind::Percent);
                case '^':
                    return createToken(TokenKind::Caret);
                case '|':
                    return createToken(TokenKind::Pipe);
                case '&':
                    return createToken(TokenKind::Ampersand);
                case '<':
                    return createToken(TokenKind::Less);
                case '>':
                    return createToken(TokenKind::Greater);
                case '=':
                    return createToken(TokenKind::Equal);
                case '!':
                    return createToken(TokenKind::Exclaim);
                case '+':
                    return createToken(TokenKind::Plus);
                case '-':
                    return createToken(TokenKind::Minus);
                case '.':
                    return createToken(TokenKind::Period);
                case ':':
                    return createToken(TokenKind::Colon);
                case '#':
                    return createToken(TokenKind::Hash);
            }
            return createToken(TokenKind::Invalid);
        }

        Token OperatorScanner::handleDoubleCharOperator(char first, char second) {
            switch (first) {
                case '+':
                    if (second == '+') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::PlusPlus);
                    }
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::PlusEqual);
                    }
                    break;
                case '-':
                    if (second == '-') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::MinusMinus);
                    }
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::MinusEqual);
                    }
                    if (second == '>') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::Arrow);
                    }
                    break;
                case '*':
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::StarEqual);
                    }
                    break;
                case '/':
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::SlashEqual);
                    }
                    break;
                case '%':
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::PercentEqual);
                    }
                    break;
                case '^':
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::CaretEqual);
                    }
                    break;
                case '&':
                    if (second == '&') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::AmpAmp);
                    }
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::AmpEqual);
                    }
                    break;
                case '|':
                    if (second == '|') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::PipePipe);
                    }
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::PipeEqual);
                    }
                    break;
                case '<':
                    if (second == '<') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::LessLess);
                    }
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::LessEqual);
                    }
                    break;
                case '>':
                    if (second == '>') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::GreaterGreater);
                    }
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::GreaterEqual);
                    }
                    break;
                case '=':
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::EqualEqual);
                    }
                    break;
                case '!':
                    if (second == '=') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::ExclaimEqual);
                    }
                    break;
                case ':':
                    if (second == ':') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::ColonColon);
                    }
                    break;
                case '#':
                    if (second == '#') {
                        currentPos++;
                        currentColumn++;
                        return createToken(TokenKind::HashHash);
                    }
                    break;
            }
            return createToken(TokenKind::Invalid);
        }

        Token OperatorScanner::handleTripleCharOperator(char first, char second, char third) {
            if (first == '.' && second == '.' && third == '.') {
                currentPos += 2;
                currentColumn += 2;
                return createToken(TokenKind::Ellipsis);
            }
            if (first == '<' && second == '<' && third == '<') {
                currentPos += 2;
                currentColumn += 2;
                return createToken(TokenKind::LessLessLess);
            }
            return createToken(TokenKind::Invalid);
        }

    }  // namespace frontend
}  // namespace rp
