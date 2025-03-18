#include "TokenKind.h"

#include "TokenUtils.h"

namespace rp {
    namespace frontend {
        namespace {
            // Token类型的范围定义
            enum class TokenRange {
                BaseStart = 0,
                BaseEnd = 99,
                KeywordStart = 100,
                KeywordEnd = 199,
                OperatorStart = 200,
                OperatorEnd = 299,
                DelimiterStart = 300,
                DelimiterEnd = 399,
                DirectiveStart = 400,
                DirectiveEnd = 499,
                TypeStart = 500,
                TypeEnd = 599
            };

            // 检查token是否在指定范围内
            bool isInRange(TokenKind kind, TokenRange start, TokenRange end) {
                int value = static_cast<int>(kind);
                return value >= static_cast<int>(start) && value <= static_cast<int>(end);
            }
        }  // namespace

        bool isKeyword(TokenKind kind) { return isInRange(kind, TokenRange::KeywordStart, TokenRange::KeywordEnd); }

        bool isOperator(TokenKind kind) { return isInRange(kind, TokenRange::OperatorStart, TokenRange::OperatorEnd); }

        bool isDelimiter(TokenKind kind) {
            return isInRange(kind, TokenRange::DelimiterStart, TokenRange::DelimiterEnd);
        }

        bool isDirective(TokenKind kind) {
            return isInRange(kind, TokenRange::DirectiveStart, TokenRange::DirectiveEnd);
        }

        bool isTypeKeyword(TokenKind kind) { return isInRange(kind, TokenRange::TypeStart, TokenRange::TypeEnd); }

        // 获取token的字符串表示

        // 新增：检查是否是基本token
        bool isBaseToken(TokenKind kind) { return isInRange(kind, TokenRange::BaseStart, TokenRange::BaseEnd); }

        // 新增：检查token是否有效
        bool isValidToken(TokenKind kind) {
            return isBaseToken(kind) || isKeyword(kind) || isOperator(kind) || isDelimiter(kind) || isDirective(kind) ||
                   isTypeKeyword(kind);
        }

        // 新增：获取token的范围类别
        TokenCategory getTokenCategory(TokenKind kind) {
            if (isBaseToken(kind)) {
                return TokenCategory::Special;
            } else if (isKeyword(kind)) {
                return TokenCategory::Keyword;
            } else if (isOperator(kind)) {
                return TokenCategory::Operator;
            } else if (isDelimiter(kind)) {
                return TokenCategory::Delimiter;
            } else if (isDirective(kind)) {
                return TokenCategory::Special;
            } else if (isTypeKeyword(kind)) {
                return TokenCategory::Keyword;
            }
            return TokenCategory::Special;
        }

    }  // namespace frontend
}  // namespace rp
