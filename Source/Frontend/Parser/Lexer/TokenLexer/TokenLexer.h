
#pragma once
#include <memory>
#include <queue>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/TokenLexer/CommentHandler.h"
#include "Frontend/Parser/Lexer/TokenLexer/CoroutineTokenHandler.h"
#include "Frontend/Parser/Lexer/TokenLexer/LambdaTokenHandler.h"
#include "Frontend/Parser/Lexer/TokenLexer/PreprocessorHandler.h"
#include "Frontend/Parser/Lexer/TokenLexer/TemplateTokenHandler.h"

namespace rp {
    namespace frontend {

        class TokenLexer {
          public:
            explicit TokenLexer(std::shared_ptr<Lexer> baseLexer);

            // 获取下一个token
            Token nextToken();

            // 预览下一个token
            Token peekToken();
            Token peekToken(size_t n);

            // 将token放回缓冲区
            void ungetToken(Token&& token);

          private:
            std::shared_ptr<Lexer> lexer;
            std::queue<Token> tokenBuffer;

            // 上下文标志
            bool isInTemplateContext = false;
            bool isInCoroutineContext = false;
            bool isInLambdaContext = false;

            // 各种处理器
            std::unique_ptr<CommentHandler> commentHandler;
            std::unique_ptr<TemplateTokenHandler> templateHandler;
            std::unique_ptr<CoroutineTokenHandler> coroutineHandler;
            std::unique_ptr<LambdaTokenHandler> lambdaHandler;
            std::unique_ptr<PreprocessorHandler> preprocessorHandler;
        };

    }  // namespace frontend
}  // namespace rp
