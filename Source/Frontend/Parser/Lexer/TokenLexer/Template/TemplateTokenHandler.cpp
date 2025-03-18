#include "Frontend/Parser/Lexer/TokenLexer/Template/TemplateTokenHandler.h"

namespace rp {
    namespace frontend {

        TemplateTokenHandler::TemplateTokenHandler(std::shared_ptr<Lexer> lexer) : lexer(lexer) {}

        Token TemplateTokenHandler::handleTemplateTokens(const Token &current) {
            // 处理模板相关的特殊token序列
            if (current.kind == TokenKind::Less) {
                const Token &next = lexer->peekToken();
                if (next.kind == TokenKind::Less) {
                    const Token &next2 = lexer->peekToken();
                    if (next2.kind == TokenKind::Less) {
                        // 处理 <<< 运算符
                        lexer->nextToken();  // 消费第二个 <
                        lexer->nextToken();  // 消费第三个 <
                        Token result(TokenKind::LessLessLess);
                        result.text = "<<<";
                        result.line = current.line;
                        result.column = current.column;
                        result.filename = current.filename;
                        return result;
                    }
                }
            } else if (current.kind == TokenKind::Greater) {
                const Token &next = lexer->peekToken();
                if (next.kind == TokenKind::Greater) {
                    const Token &next2 = lexer->peekToken();
                    if (next2.kind == TokenKind::Greater) {
                        // 处理 >>> 运算符
                        lexer->nextToken();  // 消费第二个 >
                        lexer->nextToken();  // 消费第三个 >
                        Token result(TokenKind::GreaterGreaterGreater);
                        result.text = ">>>";
                        result.line = current.line;
                        result.column = current.column;
                        result.filename = current.filename;
                        return result;
                    }
                }
            }

            // 创建新的Token并返回
            Token result(current.kind);
            result.text = current.text;
            result.line = current.line;
            result.column = current.column;
            result.filename = current.filename;
            return result;
        }

    }  // namespace frontend
}  // namespace rp
