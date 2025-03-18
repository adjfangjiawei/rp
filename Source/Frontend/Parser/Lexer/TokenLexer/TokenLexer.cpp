
#include "Frontend/Parser/Lexer/TokenLexer/TokenLexer.h"

namespace rp {
    namespace frontend {

        TokenLexer::TokenLexer(std::shared_ptr<Lexer> baseLexer)
            : lexer(baseLexer),
              commentHandler(std::make_unique<CommentHandler>(baseLexer)),
              templateHandler(std::make_unique<TemplateTokenHandler>(baseLexer)),
              coroutineHandler(std::make_unique<CoroutineTokenHandler>(baseLexer)),
              lambdaHandler(std::make_unique<LambdaTokenHandler>(baseLexer)),
              preprocessorHandler(std::make_unique<PreprocessorHandler>(baseLexer)) {}

        Token TokenLexer::nextToken() {
            // 如果缓冲区有token，先返回缓冲的token
            if (!tokenBuffer.empty()) {
                Token token = std::move(tokenBuffer.front());
                tokenBuffer.pop();
                return std::move(token);
            }

            // 获取基础lexer的下一个token
            Token token = std::move(lexer->nextToken());

            // 处理特殊情况
            if (commentHandler->isCommentStart(token)) {
                commentHandler->skipComment();
                return nextToken();
            }

            if (preprocessorHandler->isPreprocessorDirective(token)) {
                return std::move(preprocessorHandler->handlePreprocessorDirective());
            }

            // 根据上下文处理特殊token序列
            if (isInTemplateContext) {
                token = std::move(templateHandler->handleTemplateTokens(std::move(token)));
            }
            if (isInCoroutineContext) {
                token = std::move(coroutineHandler->handleCoroutineTokens(std::move(token)));
            }
            if (isInLambdaContext) {
                token = std::move(lambdaHandler->handleLambdaTokens(std::move(token)));
            }

            return std::move(token);
        }

        Token TokenLexer::peekToken() { return peekToken(1); }

        Token TokenLexer::peekToken(size_t n) {
            // 确保缓冲区有足够的token
            while (tokenBuffer.size() < n) {
                Token token = std::move(lexer->nextToken());

                // 处理注释和预处理指令
                if (commentHandler->isCommentStart(token)) {
                    commentHandler->skipComment();
                    continue;
                }

                if (preprocessorHandler->isPreprocessorDirective(token)) {
                    token = std::move(preprocessorHandler->handlePreprocessorDirective());
                }

                // 处理上下文相关的token
                if (isInTemplateContext) {
                    token = std::move(templateHandler->handleTemplateTokens(std::move(token)));
                }
                if (isInCoroutineContext) {
                    token = std::move(coroutineHandler->handleCoroutineTokens(std::move(token)));
                }
                if (isInLambdaContext) {
                    token = std::move(lambdaHandler->handleLambdaTokens(std::move(token)));
                }

                tokenBuffer.push(std::move(token));
            }

            // 创建一个临时队列来遍历tokens
            std::queue<Token> tempBuffer;
            Token result(TokenKind::Invalid);
            size_t count = 0;

            // 将tokens移动到临时队列，同时找到第n个token
            while (!tokenBuffer.empty()) {
                count++;
                if (count == n) {
                    result = std::move(tokenBuffer.front());
                }
                tempBuffer.push(std::move(tokenBuffer.front()));
                tokenBuffer.pop();
            }

            // 将tokens移回原队列
            tokenBuffer = std::move(tempBuffer);

            return std::move(result);
        }

        void TokenLexer::ungetToken(Token&& token) {
            std::queue<Token> newBuffer;
            newBuffer.push(std::move(token));

            // 将现有缓冲区的token移动到新缓冲区
            while (!tokenBuffer.empty()) {
                newBuffer.push(std::move(tokenBuffer.front()));
                tokenBuffer.pop();
            }

            tokenBuffer = std::move(newBuffer);
        }

    }  // namespace frontend
}  // namespace rp
