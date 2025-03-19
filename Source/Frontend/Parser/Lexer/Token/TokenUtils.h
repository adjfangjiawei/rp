#pragma once

#include <string>

#include "TokenCategory.h"
#include "TokenKind.h"

namespace rp {
    namespace frontend {
        namespace TokenUtils {

            // 基本类型检查
            bool isKeyword(TokenKind kind);
            bool isOperator(TokenKind kind);
            bool isDelimiter(TokenKind kind);
            bool isDirective(TokenKind kind);
            bool isTypeKeyword(TokenKind kind);
            bool isCpp11Keyword(TokenKind kind);
            bool isCpp14Keyword(TokenKind kind);
            bool isCpp17Keyword(TokenKind kind);
            bool isCpp20Keyword(TokenKind kind);
            bool isPreprocessorDirective(TokenKind kind);

            // 字符串字面量相关检查
            bool isStringLiteral(TokenKind kind);
            bool isRawStringLiteral(TokenKind kind);
            bool isWideStringLiteral(TokenKind kind);
            bool isUTF8StringLiteral(TokenKind kind);
            bool isUTF16StringLiteral(TokenKind kind);
            bool isUTF32StringLiteral(TokenKind kind);
            bool isStringPrefix(TokenKind kind);
            bool isStringError(TokenKind kind);

            // 获取Token的字符串表示
            const char* getTokenKindName(TokenKind kind);
            std::string getTokenText(TokenKind kind);

            // 字符串前缀处理
            bool isValidStringPrefix(const std::string& prefix);
            TokenKind getStringPrefixKind(const std::string& prefix);
            TokenKind getStringLiteralKind(const std::string& prefix);

            // 错误处理
            bool isErrorToken(TokenKind kind);
            const char* getErrorMessage(TokenKind kind);

            // 语法结构相关检查
            bool isStatementTerminator(TokenKind kind);
            bool isBlockStart(TokenKind kind);
            bool isBlockEnd(TokenKind kind);
            bool isRecoveryToken(TokenKind kind);

            // 类型和存储类说明符
            bool isTypeSpecifier(TokenKind kind);
            bool isStorageClassSpecifier(TokenKind kind);

            // 运算符相关
            bool isAssignmentOperator(TokenKind kind);
            bool isComparisonOperator(TokenKind kind);
            bool isIncrementDecrementOperator(TokenKind kind);

            // 模板和访问说明符
            bool isTemplateToken(TokenKind kind);
            bool isAccessSpecifier(TokenKind kind);

            // Token分类
            TokenCategory getCategory(TokenKind kind);

        }  // namespace TokenUtils
    }  // namespace frontend
}  // namespace rp
