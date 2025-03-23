#include "TokenKind.h"

#include "TokenUtils.h"

namespace rp {
    namespace frontend {

        bool isBaseToken(TokenKind kind) { return kind > TokenKind::FirstBaseToken && kind < TokenKind::LastBaseToken; }

        bool isStringLiteralToken(TokenKind kind) {
            return kind > TokenKind::FirstStringLiteral && kind < TokenKind::LastStringLiteral;
        }

        bool isKeyword(TokenKind kind) { return kind > TokenKind::FirstKeyword && kind < TokenKind::LastKeyword; }

        bool isOperator(TokenKind kind) { return kind > TokenKind::FirstOperator && kind < TokenKind::LastOperator; }

        bool isDelimiter(TokenKind kind) { return kind > TokenKind::FirstDelimiter && kind < TokenKind::LastDelimiter; }

        bool isDirective(TokenKind kind) { return kind > TokenKind::FirstDirective && kind < TokenKind::LastDirective; }

        // 获取token的分类
        TokenCategory getTokenCategory(TokenKind kind) {
            if (isBaseToken(kind)) {
                if (kind == TokenKind::EndOfFile || kind == TokenKind::Invalid) {
                    return TokenCategory::Special;
                }
                if (kind == TokenKind::Identifier) {
                    return TokenCategory::Identifier;
                }
                if (kind == TokenKind::NumberLiteral || kind == TokenKind::CharLiteral) {
                    return TokenCategory::Literal;
                }
            }

            if (isStringLiteralToken(kind)) {
                return TokenCategory::Literal;
            }

            if (isKeyword(kind)) {
                return TokenCategory::Keyword;
            }

            if (isOperator(kind)) {
                return TokenCategory::Operator;
            }

            if (isDelimiter(kind)) {
                return TokenCategory::Delimiter;
            }

            if (isDirective(kind)) {
                return TokenCategory::Special;
            }

            return TokenCategory::Special;
        }

        // 获取token的字符串表示
        const char* getTokenKindName(TokenKind kind) {
            switch (kind) {
#define TOKEN(name, str)  \
    case TokenKind::name: \
        return str;
#define KEYWORD(name, str)          \
    case TokenKind::Keyword_##name: \
        return str;
#define OPERATOR(name, str) \
    case TokenKind::name:   \
        return str;
#define DELIMITER(name, str) \
    case TokenKind::name:    \
        return str;
#define DIRECTIVE(name, str)          \
    case TokenKind::Directive_##name: \
        return str;
#define TYPE(name, str)             \
    case TokenKind::Keyword_##name: \
        return str;
#include "TokenKind.def"
#undef TOKEN
#undef KEYWORD
#undef OPERATOR
#undef DELIMITER
#undef DIRECTIVE
#undef TYPE
                default:
                    return "unknown";
            }
        }

    }  // namespace frontend
}  // namespace rp
