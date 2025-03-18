
#include "Frontend/Parser/Lexer/TokenLexer/CommentHandler.h"

namespace rp {
    namespace frontend {

        CommentHandler::CommentHandler(std::shared_ptr<Lexer> lexer) : lexer(lexer) {}

        void CommentHandler::skipComment() {
            Token current = lexer->nextToken();
            if (current.kind == TokenKind::Slash) {
                Token next = lexer->peekToken();
                if (next.kind == TokenKind::Slash) {
                    skipLineComment();
                } else if (next.kind == TokenKind::Star) {
                    skipBlockComment();
                }
            }
        }

        void CommentHandler::skipLineComment() {
            // 跳过直到行尾
            while (true) {
                Token token = lexer->nextToken();
                if (token.kind == TokenKind::EndOfFile || token.text.find('\n') != std::string::npos) {
                    break;
                }
            }
        }

        void CommentHandler::skipBlockComment() {
            // 跳过直到找到配对的注释结束符
            int nestingLevel = 1;
            while (nestingLevel > 0) {
                Token token = lexer->nextToken();
                if (token.kind == TokenKind::EndOfFile) {
                    // 错误：未闭合的块注释
                    break;
                }

                if (token.kind == TokenKind::Slash && lexer->peekToken().kind == TokenKind::Star) {
                    // 嵌套注释开始
                    lexer->nextToken();  // 消费 *
                    nestingLevel++;
                } else if (token.kind == TokenKind::Star && lexer->peekToken().kind == TokenKind::Slash) {
                    // 注释结束
                    lexer->nextToken();  // 消费 /
                    nestingLevel--;
                }
            }
        }

        bool CommentHandler::isCommentStart(const Token &token) const {
            if (token.kind == TokenKind::Slash) {
                Token next = lexer->peekToken();
                return next.kind == TokenKind::Slash || next.kind == TokenKind::Star;
            }
            return false;
        }

    }  // namespace frontend
}  // namespace rp
