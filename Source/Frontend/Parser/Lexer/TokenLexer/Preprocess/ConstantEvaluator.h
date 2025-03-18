#pragma once

#include <stack>
#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class ConstantEvaluator {
          public:
            explicit ConstantEvaluator(DiagnosticEngine* diagEngine);

            // 求值入口
            bool evaluate(const std::vector<Token>& tokens, int64_t& result);

            // 检查是否是常量表达式
            bool isConstantExpression(const std::vector<Token>& tokens) const;

          private:
            DiagnosticEngine* diagnostics;

            // 运算符优先级
            enum class Precedence {
                None,
                LogicalOr,       // ||
                LogicalAnd,      // &&
                BitwiseOr,       // |
                BitwiseXor,      // ^
                BitwiseAnd,      // &
                Equality,        // ==, !=
                Relational,      // <, >, <=, >=
                Shift,           // <<, >>
                Additive,        // +, -
                Multiplicative,  // *, /, %
                Unary            // !, ~, +, -
            };

            // 辅助函数
            bool parseExpression(const std::vector<Token>& tokens, size_t& pos, int64_t& result);
            bool parseTerm(const std::vector<Token>& tokens, size_t& pos, int64_t& result);
            bool parseFactor(const std::vector<Token>& tokens, size_t& pos, int64_t& result);
            bool parseUnary(const std::vector<Token>& tokens, size_t& pos, int64_t& result);
            bool parsePrimary(const std::vector<Token>& tokens, size_t& pos, int64_t& result);

            // 运算符处理
            Precedence getOperatorPrecedence(const Token& token) const;
            bool applyOperator(const Token& op, int64_t left, int64_t right, int64_t& result);
            bool applyUnaryOperator(const Token& op, int64_t operand, int64_t& result);

            // 错误处理
            void reportError(const std::string& message, const Token& token);
        };

    }  // namespace frontend
}  // namespace rp
