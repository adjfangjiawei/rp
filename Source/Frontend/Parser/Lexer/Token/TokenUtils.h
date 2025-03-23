#pragma once

#include <string>

#include "TokenCategory.h"
#include "TokenKind.h"

namespace rp {
    namespace frontend {
        namespace TokenUtils {
            // Token类型判断基础函数
            bool isBaseToken(TokenKind kind);
            bool isStringLiteralToken(TokenKind kind);
            bool isKeyword(TokenKind kind);
            bool isOperator(TokenKind kind);
            bool isDelimiter(TokenKind kind);
            bool isDirective(TokenKind kind);

            // 字符串字面量相关函数
            bool isRawStringLiteral(TokenKind kind);
            bool isWideStringLiteral(TokenKind kind);
            bool isUTF8StringLiteral(TokenKind kind);
            bool isUTF16StringLiteral(TokenKind kind);
            bool isUTF32StringLiteral(TokenKind kind);
            bool isStringPrefix(TokenKind kind);
            bool isStringError(TokenKind kind);

            // 字符串前缀处理
            bool isValidStringPrefix(const std::string& prefix);
            TokenKind getStringPrefixKind(const std::string& prefix);
            TokenKind getStringLiteralKind(const std::string& prefix);

            // 错误处理
            bool isErrorToken(TokenKind kind);
            const char* getErrorMessage(TokenKind kind);
            const char* getErrorDetail(TokenKind kind);

            // 语法结构相关
            bool isStatementTerminator(TokenKind kind);
            bool isCompoundStatementToken(TokenKind kind);
            bool isDeclarationSpecifier(TokenKind kind);
            bool isPreprocessorDirective(TokenKind kind);  // 新增
            bool isBlockStart(TokenKind kind);             // 新增
            bool isBlockEnd(TokenKind kind);               // 新增
            bool isRecoveryToken(TokenKind kind);          // 新增

            // 类型系统相关
            bool isTypeSpecifier(TokenKind kind);
            bool isBasicTypeSpecifier(TokenKind kind);
            bool isCharTypeSpecifier(TokenKind kind);
            bool isTypeModifier(TokenKind kind);
            bool isStorageClassSpecifier(TokenKind kind);

            // 运算符相关
            bool isAssignmentOperator(TokenKind kind);
            bool isArithmeticOperator(TokenKind kind);
            bool isBitwiseOperator(TokenKind kind);
            bool isComparisonOperator(TokenKind kind);
            bool isLogicalOperator(TokenKind kind);
            bool isUnaryOperator(TokenKind kind);
            bool isIncrementDecrementOperator(TokenKind kind);

            // 面向对象相关
            bool isAccessSpecifier(TokenKind kind);
            bool isClassSpecifier(TokenKind kind);
            bool isMemberAccessToken(TokenKind kind);

            // 现代C++特性
            bool isTemplateToken(TokenKind kind);
            bool isConstraintToken(TokenKind kind);
            bool isLambdaToken(TokenKind kind);
            bool isCoroutineToken(TokenKind kind);
            bool isModuleToken(TokenKind kind);

            // 属性和说明符
            bool isAttributeSpecifier(TokenKind kind);
            bool isConstantSpecifier(TokenKind kind);
            bool isFunctionSpecifier(TokenKind kind);

            // C++版本特性
            bool isCpp11Keyword(TokenKind kind);
            bool isCpp14Keyword(TokenKind kind);
            bool isCpp17Keyword(TokenKind kind);
            bool isCpp20Keyword(TokenKind kind);

            // Token分类和名称
            TokenCategory getCategory(TokenKind kind);
            const char* getTokenKindName(TokenKind kind);

            // 表达式相关
            bool isExpressionStart(TokenKind kind);
            bool isInitializationToken(TokenKind kind);
        }  // namespace TokenUtils
    }  // namespace frontend
}  // namespace rp
