
#include "Frontend/Parser/Lexer/TokenLexer/PreprocessorHandler.h"

namespace rp {
    namespace frontend {

        PreprocessorHandler::PreprocessorHandler(std::shared_ptr<Lexer> lexer) : lexer(lexer) {}

        Token PreprocessorHandler::handlePreprocessorDirective() {
            // 跳过预处理指令
            while (true) {
                Token token = lexer->nextToken();
                if (token.kind == TokenKind::EndOfFile || token.text.find('\n') != std::string::npos) {
                    break;
                }
            }

            // 返回下一个有效token
            return lexer->nextToken();
        }

        bool PreprocessorHandler::isPreprocessorDirective(const Token &token) const {
            return token.kind == TokenKind::Invalid && token.text == "#";
        }

    }  // namespace frontend
}  // namespace rp
