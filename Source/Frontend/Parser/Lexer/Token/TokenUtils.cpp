#include "TokenUtils.h"

#include <unordered_map>

namespace rp {
    namespace frontend {
        namespace TokenUtils {

            bool isStringLiteral(TokenKind kind) {
                switch (kind) {
                    case TokenKind::StringLiteral:
                    case TokenKind::RawStringLiteral:
                    case TokenKind::WideStringLiteral:
                    case TokenKind::UTF8StringLiteral:
                    case TokenKind::UTF16StringLiteral:
                    case TokenKind::UTF32StringLiteral:
                        return true;
                    default:
                        return false;
                }
            }

            bool isRawStringLiteral(TokenKind kind) { return kind == TokenKind::RawStringLiteral; }

            bool isWideStringLiteral(TokenKind kind) { return kind == TokenKind::WideStringLiteral; }

            bool isUTF8StringLiteral(TokenKind kind) { return kind == TokenKind::UTF8StringLiteral; }

            bool isUTF16StringLiteral(TokenKind kind) { return kind == TokenKind::UTF16StringLiteral; }

            bool isUTF32StringLiteral(TokenKind kind) { return kind == TokenKind::UTF32StringLiteral; }

            bool isStringPrefix(TokenKind kind) {
                switch (kind) {
                    case TokenKind::StringPrefix_L:
                    case TokenKind::StringPrefix_u8:
                    case TokenKind::StringPrefix_u:
                    case TokenKind::StringPrefix_U:
                    case TokenKind::StringPrefix_R:
                        return true;
                    default:
                        return false;
                }
            }

            bool isStringError(TokenKind kind) {
                switch (kind) {
                    case TokenKind::StringLiteral_Unterminated:
                    case TokenKind::StringLiteral_InvalidEscape:
                    case TokenKind::StringLiteral_InvalidUTF8:
                    case TokenKind::StringLiteral_InvalidDelimiter:
                        return true;
                    default:
                        return false;
                }
            }

            static const std::unordered_map<std::string, TokenKind> stringPrefixMap = {
                {"L", TokenKind::StringPrefix_L},
                {"u8", TokenKind::StringPrefix_u8},
                {"u", TokenKind::StringPrefix_u},
                {"U", TokenKind::StringPrefix_U},
                {"R", TokenKind::StringPrefix_R}};

            static const std::unordered_map<std::string, TokenKind> stringLiteralMap = {
                {"L", TokenKind::WideStringLiteral},
                {"u8", TokenKind::UTF8StringLiteral},
                {"u", TokenKind::UTF16StringLiteral},
                {"U", TokenKind::UTF32StringLiteral},
                {"R", TokenKind::RawStringLiteral},
                {"", TokenKind::StringLiteral}};

            bool isValidStringPrefix(const std::string& prefix) {
                return stringPrefixMap.find(prefix) != stringPrefixMap.end();
            }

            TokenKind getStringPrefixKind(const std::string& prefix) {
                auto it = stringPrefixMap.find(prefix);
                return it != stringPrefixMap.end() ? it->second : TokenKind::Invalid;
            }

            TokenKind getStringLiteralKind(const std::string& prefix) {
                auto it = stringLiteralMap.find(prefix);
                return it != stringLiteralMap.end() ? it->second : TokenKind::Invalid;
            }

            bool isErrorToken(TokenKind kind) { return kind == TokenKind::Invalid || isStringError(kind); }

            const char* getErrorMessage(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Invalid:
                        return "Invalid token";
                    case TokenKind::StringLiteral_Unterminated:
                        return "Unterminated string literal";
                    case TokenKind::StringLiteral_InvalidEscape:
                        return "Invalid escape sequence in string literal";
                    case TokenKind::StringLiteral_InvalidUTF8:
                        return "Invalid UTF-8 sequence in string literal";
                    case TokenKind::StringLiteral_InvalidDelimiter:
                        return "Invalid delimiter in raw string literal";
                    default:
                        return "Unknown error";
                }
            }

            // 实现其他已存在的函数...
            bool isKeyword(TokenKind kind) {
                return (static_cast<int>(kind) >= static_cast<int>(TokenKind::Keyword_Auto) &&
                        static_cast<int>(kind) <= static_cast<int>(TokenKind::Keyword_Export));
            }

            bool isOperator(TokenKind kind) {
                return (static_cast<int>(kind) >= static_cast<int>(TokenKind::Plus) &&
                        static_cast<int>(kind) <= static_cast<int>(TokenKind::Arrow));
            }

            bool isDelimiter(TokenKind kind) {
                return (static_cast<int>(kind) >= static_cast<int>(TokenKind::LParen) &&
                        static_cast<int>(kind) <= static_cast<int>(TokenKind::RBrace));
            }

            bool isDirective(TokenKind kind) {
                return (static_cast<int>(kind) >= static_cast<int>(TokenKind::Directive_Include) &&
                        static_cast<int>(kind) <= static_cast<int>(TokenKind::Directive_Warning));
            }

            bool isTypeKeyword(TokenKind kind) {
                return (static_cast<int>(kind) >= static_cast<int>(TokenKind::Keyword_Void) &&
                        static_cast<int>(kind) <= static_cast<int>(TokenKind::Keyword_Bool));
            }

            bool isCpp11Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Alignas:
                    case TokenKind::Keyword_Alignof:
                    case TokenKind::Keyword_Char16_t:
                    case TokenKind::Keyword_Char32_t:
                    case TokenKind::Keyword_Constexpr:
                    case TokenKind::Keyword_Decltype:
                    case TokenKind::Keyword_Noexcept:
                    case TokenKind::Keyword_Nullptr:
                    case TokenKind::Keyword_Static_assert:
                    case TokenKind::Keyword_Thread_local:
                        return true;
                    default:
                        return false;
                }
            }

            bool isCpp14Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Deprecated:
                        return true;
                    default:
                        return false;
                }
            }

            bool isCpp17Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Fallthrough:
                    case TokenKind::Keyword_Nodiscard:
                    case TokenKind::Keyword_Maybe_unused:
                        return true;
                    default:
                        return false;
                }
            }

            bool isCpp20Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Concept:
                    case TokenKind::Keyword_Consteval:
                    case TokenKind::Keyword_Constinit:
                    case TokenKind::Keyword_co_await:
                    case TokenKind::Keyword_co_return:
                    case TokenKind::Keyword_co_yield:
                    case TokenKind::Keyword_Requires:
                        return true;
                    default:
                        return false;
                }
            }

            bool isStatementTerminator(TokenKind kind) { return kind == TokenKind::Semicolon; }

            bool isBlockStart(TokenKind kind) { return kind == TokenKind::LBrace; }

            bool isBlockEnd(TokenKind kind) { return kind == TokenKind::RBrace; }

            bool isRecoveryToken(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Semicolon:
                    case TokenKind::LBrace:
                    case TokenKind::RBrace:
                        return true;
                    default:
                        return false;
                }
            }

            bool isTypeSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Void:
                    case TokenKind::Keyword_Char:
                    case TokenKind::Keyword_Short:
                    case TokenKind::Keyword_Int:
                    case TokenKind::Keyword_Long:
                    case TokenKind::Keyword_Float:
                    case TokenKind::Keyword_Double:
                    case TokenKind::Keyword_Signed:
                    case TokenKind::Keyword_Unsigned:
                    case TokenKind::Keyword_Bool:
                    case TokenKind::Keyword_Char16_t:
                    case TokenKind::Keyword_Char32_t:
                    case TokenKind::Keyword_Char8_t:
                        return true;
                    default:
                        return false;
                }
            }

            bool isStorageClassSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Static:
                    case TokenKind::Keyword_Extern:
                    case TokenKind::Keyword_Thread_local:
                    case TokenKind::Keyword_Mutable:
                    case TokenKind::Keyword_Register:
                    case TokenKind::Keyword_Auto:
                        return true;
                    default:
                        return false;
                }
            }

            bool isAssignmentOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Equal:
                    case TokenKind::PlusEqual:
                    case TokenKind::MinusEqual:
                    case TokenKind::StarEqual:
                    case TokenKind::SlashEqual:
                    case TokenKind::PercentEqual:
                    case TokenKind::AmpEqual:
                    case TokenKind::PipeEqual:
                    case TokenKind::CaretEqual:
                    case TokenKind::LessEqual:
                    case TokenKind::GreaterEqual:
                        return true;
                    default:
                        return false;
                }
            }

            bool isComparisonOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::EqualEqual:
                    case TokenKind::ExclaimEqual:
                    case TokenKind::Less:
                    case TokenKind::Greater:
                    case TokenKind::LessEqual:
                    case TokenKind::GreaterEqual:
                    case TokenKind::Spaceship:
                        return true;
                    default:
                        return false;
                }
            }

            bool isIncrementDecrementOperator(TokenKind kind) {
                return kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
            }

            bool isTemplateToken(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Template:
                    case TokenKind::LessLessLess:
                    case TokenKind::GreaterGreaterGreater:
                        return true;
                    default:
                        return false;
                }
            }

            bool isAccessSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Public:
                    case TokenKind::Keyword_Private:
                    case TokenKind::Keyword_Protected:
                        return true;
                    default:
                        return false;
                }
            }

            TokenCategory getCategory(TokenKind kind) {
                if (kind == TokenKind::EndOfFile || kind == TokenKind::Invalid) {
                    return TokenCategory::Special;
                }
                if (kind == TokenKind::NumberLiteral || isStringLiteral(kind)) {
                    return TokenCategory::Literal;
                }
                if (kind == TokenKind::Identifier) {
                    return TokenCategory::Identifier;
                }
                if (TokenUtils::isKeyword(kind)) {
                    return TokenCategory::Keyword;
                }
                if (TokenUtils::isOperator(kind)) {
                    return TokenCategory::Operator;
                }
                if (TokenUtils::isDelimiter(kind)) {
                    return TokenCategory::Delimiter;
                }
                return TokenCategory::Special;
            }

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

        }  // namespace TokenUtils
    }  // namespace frontend
}  // namespace rp
