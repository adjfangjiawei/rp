#include "ConstantEvaluator.h"

#include <cctype>
#include <limits>

namespace rp {
    namespace frontend {

        ConstantEvaluator::ConstantEvaluator(DiagnosticEngine* diagEngine) : diagnostics(diagEngine) {}

        bool ConstantEvaluator::evaluate(const std::vector<Token>& tokens, int64_t& result) {
            if (tokens.empty()) {
                reportError("Empty expression", Token());
                return false;
            }

            size_t pos = 0;
            if (!parseExpression(tokens, pos, result)) {
                return false;
            }

            if (pos < tokens.size()) {
                reportError("Unexpected token after expression", tokens[pos]);
                return false;
            }

            return true;
        }

        bool ConstantEvaluator::isConstantExpression(const std::vector<Token>& tokens) const {
            for (const auto& token : tokens) {
                switch (token.kind) {
                    case TokenKind::NumberLiteral:
                    case TokenKind::Plus:
                    case TokenKind::Minus:
                    case TokenKind::Star:
                    case TokenKind::Slash:
                    case TokenKind::Percent:
                    case TokenKind::Less:
                    case TokenKind::Greater:
                    case TokenKind::LessEqual:
                    case TokenKind::GreaterEqual:
                    case TokenKind::EqualEqual:
                    case TokenKind::ExclaimEqual:
                    case TokenKind::Ampersand:
                    case TokenKind::Pipe:
                    case TokenKind::Caret:
                    case TokenKind::AmpAmp:
                    case TokenKind::PipePipe:
                    case TokenKind::Exclaim:
                    case TokenKind::Tilde:
                    case TokenKind::LParen:
                    case TokenKind::RParen:
                        continue;
                    default:
                        return false;
                }
            }
            return true;
        }

        bool ConstantEvaluator::parseExpression(const std::vector<Token>& tokens, size_t& pos, int64_t& result) {
            // 解析第一个操作数
            if (!parseTerm(tokens, pos, result)) {
                return false;
            }

            while (pos < tokens.size()) {
                const Token& op = tokens[pos];
                Precedence prec = getOperatorPrecedence(op);

                if (prec == Precedence::None) {
                    break;
                }

                ++pos;
                int64_t right;
                if (!parseTerm(tokens, pos, right)) {
                    return false;
                }

                if (!applyOperator(op, result, right, result)) {
                    return false;
                }
            }

            return true;
        }

        bool ConstantEvaluator::parseTerm(const std::vector<Token>& tokens, size_t& pos, int64_t& result) {
            // 解析一元运算符
            if (pos < tokens.size()) {
                const Token& token = tokens[pos];
                if (token.kind == TokenKind::Plus || token.kind == TokenKind::Minus ||
                    token.kind == TokenKind::Exclaim || token.kind == TokenKind::Tilde) {
                    ++pos;
                    if (!parseTerm(tokens, pos, result)) {
                        return false;
                    }
                    return applyUnaryOperator(token, result, result);
                }
            }

            return parsePrimary(tokens, pos, result);
        }

        bool ConstantEvaluator::parsePrimary(const std::vector<Token>& tokens, size_t& pos, int64_t& result) {
            if (pos >= tokens.size()) {
                reportError("Unexpected end of expression", Token());
                return false;
            }

            const Token& token = tokens[pos];

            if (token.kind == TokenKind::LParen) {
                ++pos;
                if (!parseExpression(tokens, pos, result)) {
                    return false;
                }

                if (pos >= tokens.size() || tokens[pos].kind != TokenKind::RParen) {
                    reportError("Expected closing parenthesis", token);
                    return false;
                }

                ++pos;
                return true;
            }

            if (token.kind == TokenKind::NumberLiteral) {
                try {
                    // 处理不同进制的数字
                    std::string numStr(token.text);
                    int base = 10;

                    if (numStr.length() >= 2) {
                        if (numStr[0] == '0') {
                            if (numStr[1] == 'x' || numStr[1] == 'X') {
                                base = 16;
                                numStr = numStr.substr(2);
                            } else if (numStr[1] == 'b' || numStr[1] == 'B') {
                                base = 2;
                                numStr = numStr.substr(2);
                            } else if (isdigit(numStr[1])) {
                                base = 8;
                                numStr = numStr.substr(1);
                            }
                        }
                    }

                    result = std::stoll(numStr, nullptr, base);
                    ++pos;
                    return true;
                } catch (const std::exception&) {
                    reportError("Invalid number literal", token);
                    return false;
                }
            }

            reportError("Expected number or parenthesized expression", token);
            return false;
        }

        ConstantEvaluator::Precedence ConstantEvaluator::getOperatorPrecedence(const Token& token) const {
            switch (token.kind) {
                case TokenKind::PipePipe:
                    return Precedence::LogicalOr;
                case TokenKind::AmpAmp:
                    return Precedence::LogicalAnd;
                case TokenKind::Pipe:
                    return Precedence::BitwiseOr;
                case TokenKind::Caret:
                    return Precedence::BitwiseXor;
                case TokenKind::Ampersand:
                    return Precedence::BitwiseAnd;
                case TokenKind::EqualEqual:
                case TokenKind::ExclaimEqual:
                    return Precedence::Equality;
                case TokenKind::Less:
                case TokenKind::Greater:
                case TokenKind::LessEqual:
                case TokenKind::GreaterEqual:
                    return Precedence::Relational;
                case TokenKind::LessLess:
                case TokenKind::GreaterGreater:
                    return Precedence::Shift;
                case TokenKind::Plus:
                case TokenKind::Minus:
                    return Precedence::Additive;
                case TokenKind::Star:
                case TokenKind::Slash:
                case TokenKind::Percent:
                    return Precedence::Multiplicative;
                default:
                    return Precedence::None;
            }
        }

        bool ConstantEvaluator::applyOperator(const Token& op, int64_t left, int64_t right, int64_t& result) {
            switch (op.kind) {
                case TokenKind::Plus:
                    result = left + right;
                    break;
                case TokenKind::Minus:
                    result = left - right;
                    break;
                case TokenKind::Star:
                    result = left * right;
                    break;
                case TokenKind::Slash:
                    if (right == 0) {
                        reportError("Division by zero", op);
                        return false;
                    }
                    result = left / right;
                    break;
                case TokenKind::Percent:
                    if (right == 0) {
                        reportError("Modulo by zero", op);
                        return false;
                    }
                    result = left % right;
                    break;
                case TokenKind::Less:
                    result = left < right;
                    break;
                case TokenKind::Greater:
                    result = left > right;
                    break;
                case TokenKind::LessEqual:
                    result = left <= right;
                    break;
                case TokenKind::GreaterEqual:
                    result = left >= right;
                    break;
                case TokenKind::EqualEqual:
                    result = left == right;
                    break;
                case TokenKind::ExclaimEqual:
                    result = left != right;
                    break;
                case TokenKind::Ampersand:
                    result = left & right;
                    break;
                case TokenKind::Pipe:
                    result = left | right;
                    break;
                case TokenKind::Caret:
                    result = left ^ right;
                    break;
                case TokenKind::LessLess:
                    if (right < 0 || right >= 64) {
                        reportError("Invalid shift amount", op);
                        return false;
                    }
                    result = left << right;
                    break;
                case TokenKind::GreaterGreater:
                    if (right < 0 || right >= 64) {
                        reportError("Invalid shift amount", op);
                        return false;
                    }
                    result = left >> right;
                    break;
                case TokenKind::AmpAmp:
                    result = left && right;
                    break;
                case TokenKind::PipePipe:
                    result = left || right;
                    break;
                default:
                    reportError("Unknown operator", op);
                    return false;
            }
            return true;
        }

        bool ConstantEvaluator::applyUnaryOperator(const Token& op, int64_t operand, int64_t& result) {
            switch (op.kind) {
                case TokenKind::Plus:
                    result = +operand;
                    break;
                case TokenKind::Minus:
                    result = -operand;
                    break;
                case TokenKind::Exclaim:
                    result = !operand;
                    break;
                case TokenKind::Tilde:
                    result = ~operand;
                    break;
                default:
                    reportError("Unknown unary operator", op);
                    return false;
            }
            return true;
        }

        void ConstantEvaluator::reportError(const std::string& message, const Token& token) {
            SourceLocation loc;
            loc.filename = token.filename;
            loc.line = token.line;
            loc.column = token.column;
            diagnostics->report(DiagnosticLevel::Error, loc, message);
        }

    }  // namespace frontend
}  // namespace rp
