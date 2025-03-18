#pragma once
#include <string_view>
#include <unordered_set>

#include "TokenBase.h"
#include "TokenCategory.h"
#include "TokenKind.h"

namespace rp {
    namespace frontend {

        class TokenUtils {
          public:
            // 基本类型判断函数
            static bool isKeyword(const TokenKind kind) {
                return (kind >= TokenKind::Keyword_Class && kind <= TokenKind::Keyword_Export);
            }

            static bool isOperator(const TokenKind kind) {
                return (kind >= TokenKind::Plus && kind <= TokenKind::Spaceship);
            }

            static bool isDelimiter(const TokenKind kind) {
                static const std::unordered_set<TokenKind> delimiters = {TokenKind::LParen,
                                                                         TokenKind::RParen,
                                                                         TokenKind::LBrace,
                                                                         TokenKind::RBrace,
                                                                         TokenKind::LBrace,
                                                                         TokenKind::RBrace,
                                                                         TokenKind::Semicolon,
                                                                         TokenKind::Comma,
                                                                         TokenKind::Period,
                                                                         TokenKind::Ellipsis};
                return delimiters.count(kind) > 0;
            }

            static bool isLiteral(const TokenKind kind) {
                return (kind == TokenKind::NumberLiteral || kind == TokenKind::StringLiteral ||
                        kind == TokenKind::CharLiteral);
            }

            // C++版本相关判断
            static bool isCpp11Keyword(const TokenKind kind) {
                static const std::unordered_set<TokenKind> cpp11Keywords = {TokenKind::Keyword_Alignas,
                                                                            TokenKind::Keyword_Alignof,
                                                                            TokenKind::Keyword_Char16_t,
                                                                            TokenKind::Keyword_Char32_t,
                                                                            TokenKind::Keyword_Constexpr,
                                                                            TokenKind::Keyword_Decltype,
                                                                            TokenKind::Keyword_Noexcept,
                                                                            TokenKind::Keyword_Nullptr,
                                                                            TokenKind::Keyword_Static_assert,
                                                                            TokenKind::Keyword_Thread_local};
                return cpp11Keywords.count(kind) > 0;
            }

            static bool isCpp14Keyword(const TokenKind kind) { return kind == TokenKind::Keyword_Deprecated; }

            static bool isCpp17Keyword(const TokenKind kind) {
                static const std::unordered_set<TokenKind> cpp17Keywords = {
                    TokenKind::Keyword_Fallthrough, TokenKind::Keyword_Nodiscard, TokenKind::Keyword_Maybe_unused};
                return cpp17Keywords.count(kind) > 0;
            }

            static bool isCpp20Keyword(const TokenKind kind) {
                static const std::unordered_set<TokenKind> cpp20Keywords = {TokenKind::Keyword_Char8_t,
                                                                            TokenKind::Keyword_Concept,
                                                                            TokenKind::Keyword_Consteval,
                                                                            TokenKind::Keyword_Constinit,
                                                                            TokenKind::Keyword_co_await,
                                                                            TokenKind::Keyword_co_return,
                                                                            TokenKind::Keyword_co_yield,
                                                                            TokenKind::Keyword_Requires};
                return cpp20Keywords.count(kind) > 0;
            }

            // 编译器指令相关判断
            static bool isPreprocessorDirective(const TokenKind kind) {
                return (kind >= TokenKind::Directive_Include && kind <= TokenKind::Directive_Pragma);
            }

            // 特殊类型判断
            static bool isTypeSpecifier(const TokenKind kind) {
                static const std::unordered_set<TokenKind> typeSpecifiers = {TokenKind::Keyword_Void,
                                                                             TokenKind::Keyword_Bool,
                                                                             TokenKind::Keyword_Char,
                                                                             TokenKind::Keyword_Short,
                                                                             TokenKind::Keyword_Int,
                                                                             TokenKind::Keyword_Long,
                                                                             TokenKind::Keyword_Float,
                                                                             TokenKind::Keyword_Double,
                                                                             TokenKind::Keyword_Signed,
                                                                             TokenKind::Keyword_Unsigned,
                                                                             TokenKind::Keyword_Char8_t,
                                                                             TokenKind::Keyword_Char16_t,
                                                                             TokenKind::Keyword_Char32_t};
                return typeSpecifiers.count(kind) > 0;
            }

            static bool isStorageClassSpecifier(const TokenKind kind) {
                static const std::unordered_set<TokenKind> storageSpecifiers = {TokenKind::Keyword_Auto,
                                                                                TokenKind::Keyword_Register,
                                                                                TokenKind::Keyword_Static,
                                                                                TokenKind::Keyword_Extern,
                                                                                TokenKind::Keyword_Mutable};
                return storageSpecifiers.count(kind) > 0;
            }

            // 错误恢复相关函数
            static bool isStatementTerminator(const TokenKind kind) { return kind == TokenKind::Semicolon; }

            static bool isBlockStart(const TokenKind kind) { return kind == TokenKind::LBrace; }

            static bool isBlockEnd(const TokenKind kind) { return kind == TokenKind::RBrace; }

            static bool isRecoveryToken(const TokenKind kind) {
                return isStatementTerminator(kind) || isBlockStart(kind) || isBlockEnd(kind);
            }

            // Token分类获取
            static TokenCategory getCategory(const TokenKind kind) {
                if (kind == TokenKind::EndOfFile || kind == TokenKind::Invalid) {
                    return TokenCategory::Special;
                }
                if (isLiteral(kind)) {
                    return TokenCategory::Literal;
                }
                if (kind == TokenKind::Identifier) {
                    return TokenCategory::Identifier;
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
                if (kind == TokenKind::LBrace || kind == TokenKind::RBrace) {
                    return TokenCategory::Attribute;
                }
                return TokenCategory::Special;
            }

            // 运算符相关判断
            static bool isAssignmentOperator(const TokenKind kind) {
                return kind == TokenKind::Equal || (kind >= TokenKind::PlusEqual && kind <= TokenKind::CaretEqual);
            }

            static bool isComparisonOperator(const TokenKind kind) {
                return (kind == TokenKind::EqualEqual || kind == TokenKind::ExclaimEqual || kind == TokenKind::Less ||
                        kind == TokenKind::Greater || kind == TokenKind::LessEqual || kind == TokenKind::GreaterEqual ||
                        kind == TokenKind::Spaceship);
            }

            static bool isIncrementDecrementOperator(const TokenKind kind) {
                return kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
            }

            // 访问控制相关判断
            static bool isAccessSpecifier(const TokenKind kind) {
                return (kind == TokenKind::Keyword_Public || kind == TokenKind::Keyword_Private ||
                        kind == TokenKind::Keyword_Protected);
            }

            // 模板相关判断
            static bool isTemplateToken(const TokenKind kind) {
                return (kind == TokenKind::Keyword_Template || kind == TokenKind::LessLess ||
                        kind == TokenKind::GreaterGreater);
            }
        };

    }  // namespace frontend
}  // namespace rp
