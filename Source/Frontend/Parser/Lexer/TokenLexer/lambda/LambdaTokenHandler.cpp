#include "Frontend/Parser/Lexer/TokenLexer/lambda/LambdaTokenHandler.h"

namespace rp {
    namespace frontend {

        LambdaTokenHandler::LambdaTokenHandler(std::shared_ptr<Lexer> lexer) : lexer(lexer) {}

        Token LambdaTokenHandler::handleLambdaTokens(Token&& current) {
            // 处理Lambda相关的特殊token序列
            if (current.kind == TokenKind::LSquare) {
                const Token& next = lexer->peekToken();
                if (next.kind == TokenKind::RSquare) {
                    // 处理lambda引入符 []
                    lexer->nextToken();  // 消费 ]
                    Token result(TokenKind::LambdaIntro);
                    result.text = "[]";
                    result.line = current.line;
                    result.column = current.column;
                    result.filename = current.filename;
                    return result;
                }
            } else if (current.kind == TokenKind::Equal && lexer->peekToken().kind == TokenKind::Greater) {
                // 处理lambda箭头 =>
                lexer->nextToken();  // 消费 >
                Token result(TokenKind::LambdaArrow);
                result.text = "=>";
                result.line = current.line;
                result.column = current.column;
                result.filename = current.filename;
                return result;
            }

            // 创建新的Token并返回
            Token result(current.kind);
            result.text = current.text;
            result.line = current.line;
            result.column = current.column;
            result.filename = current.filename;
            return result;
        }

        bool LambdaTokenHandler::isLambdaStart(const Token& token) { return token.kind == TokenKind::LSquare; }

        bool LambdaTokenHandler::isLambdaEnd(const Token& token) {
            return token.kind == TokenKind::RBrace;  // Lambda 表达式以右花括号结束
        }

    }  // namespace frontend
}  // namespace rp
