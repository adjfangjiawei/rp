#pragma once
#include "AST/Expr.h"
#include "Lexer/Lexer.h"
#include "../Diagnostic/Diagnostic.h"
#include <memory>
#include <map>

namespace rp
{
    namespace frontend
    {

        class ExprParser
        {
        public:
            ExprParser(Lexer &lexer, std::shared_ptr<DiagnosticEngine> diag)
                : lexer(lexer), diagnostics(diag) {}

            // 解析表达式
            std::shared_ptr<Expr> parseExpression();

            // 解析赋值表达式
            std::shared_ptr<Expr> parseAssignmentExpr();

        private:
            Lexer &lexer;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 运算符优先级
            enum class Precedence
            {
                None,           // 无优先级
                Assignment,     // =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
                Conditional,    // ?:
                LogicalOr,      // ||
                LogicalAnd,     // &&
                BitOr,          // |
                BitXor,         // ^
                BitAnd,         // &
                Equality,       // ==, !=
                Relational,     // <, <=, >, >=
                Shift,          // <<, >>
                Additive,       // +, -
                Multiplicative, // *, /, %
                Unary,          // !, ~, -, +, *, &, ++, --
                Postfix,        // (), [], ->, ., ++, --
                Primary         // 字面量，标识符，括号表达式
            };

            // 获取二元运算符优先级
            Precedence getBinaryPrecedence(TokenKind kind) const;

            // 解析各种表达式
            std::shared_ptr<Expr> parseConditionalExpr();
            std::shared_ptr<Expr> parseBinaryExpr(Precedence minPrec = Precedence::None);
            std::shared_ptr<Expr> parseUnaryExpr();
            std::shared_ptr<Expr> parsePostfixExpr();
            std::shared_ptr<Expr> parsePrimaryExpr();

            // 解析特殊表达式
            std::shared_ptr<Expr> parseLambdaExpr();
            std::shared_ptr<Expr> parseNewExpr();
            std::shared_ptr<Expr> parseDeleteExpr();
            std::shared_ptr<Expr> parseCastExpr();
            std::shared_ptr<Expr> parseParenExpr();

            // 解析字面量
            std::shared_ptr<LiteralExpr> parseIntegerLiteral();
            std::shared_ptr<LiteralExpr> parseFloatingLiteral();
            std::shared_ptr<LiteralExpr> parseStringLiteral();
            std::shared_ptr<LiteralExpr> parseCharacterLiteral();
            std::shared_ptr<LiteralExpr> parseBooleanLiteral();

            // 辅助函数
            bool isUnaryOperator(TokenKind kind) const;
            bool isBinaryOperator(TokenKind kind) const;
            bool isAssignmentOperator(TokenKind kind) const;
            BinaryExpr::Operator getBinaryOperator(TokenKind kind) const;
            UnaryExpr::Operator getUnaryOperator(TokenKind kind) const;

            // 错误处理和恢复
            void skipUntil(TokenKind kind);
            void reportError(const std::string &message);

            // 运算符映射表
            static const std::map<TokenKind, BinaryExpr::Operator> binaryOperatorMap;
            static const std::map<TokenKind, UnaryExpr::Operator> unaryOperatorMap;
        };

    } // namespace frontend
} // namespace rp
