#include "Frontend/Parser/Lexer/TokenLexer/Coroutine/CoroutineTokenHandler.h"

namespace rp {
    namespace frontend {

        CoroutineTokenHandler::CoroutineTokenHandler(std::shared_ptr<Lexer> lexer) : lexer(lexer) {}

        Token CoroutineTokenHandler::handleCoroutineTokens(const Token &current) {
            // 处理协程相关的特殊token序列
            if (current.kind == TokenKind::Identifier) {
                if (current.text == "co_await") {
                    return Token(TokenKind::Keyword_co_await);
                } else if (current.text == "co_yield") {
                    return Token(TokenKind::Keyword_co_yield);
                } else if (current.text == "co_return") {
                    return Token(TokenKind::Keyword_co_return);
                }
            }

            // 创建新的Token并移动返回
            Token result(current.kind);
            result.text = current.text;
            result.line = current.line;
            result.column = current.column;
            result.filename = current.filename;
            return result;
        }

    }  // namespace frontend
}  // namespace rp
