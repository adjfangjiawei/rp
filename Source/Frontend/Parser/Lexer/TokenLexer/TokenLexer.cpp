#include "Frontend/Parser/Lexer/TokenLexer/TokenLexer.h"

namespace rp {
    namespace frontend {

        TokenLexer::TokenLexer(std::shared_ptr<Lexer> baseLexer, DiagnosticEngine* diagEngine)
            : lexer(baseLexer),
              commentHandler(std::make_unique<CommentHandler>(baseLexer)),
              templateHandler(std::make_unique<TemplateTokenHandler>(baseLexer)),
              coroutineHandler(std::make_unique<CoroutineTokenHandler>(baseLexer)),
              lambdaHandler(std::make_unique<LambdaTokenHandler>(baseLexer)),
              preprocessorHandler(std::make_unique<PreprocessorHandler>(diagEngine)) {
            // 初始化所有上下文状态为false
            isInTemplateContext = false;
            isInCoroutineContext = false;
            isInLambdaContext = false;
        }

        Token TokenLexer::nextToken() {
            // 如果缓冲区有token，先返回缓冲的token
            if (!tokenBuffer.empty()) {
                Token token = std::move(tokenBuffer.front());
                tokenBuffer.pop();
                return std::move(token);
            }

            // 获取基础lexer的下一个token
            Token token = std::move(lexer->nextToken());

            // 处理注释
            while (commentHandler->isCommentStart(token)) {
                commentHandler->skipComment();
                token = std::move(lexer->nextToken());
            }

            // 处理预处理指令（最高优先级）
            if (preprocessorHandler->isPreprocessorDirective(token)) {
                return std::move(preprocessorHandler->handlePreprocessorDirective());
            }

            // 检测并更新上下文状态
            updateContextState(token);

            // 按照优先级顺序处理各种上下文的token
            // 1. 模板上下文（最高优先级，因为模板可能包含其他结构）
            if (isInTemplateContext) {
                token = std::move(templateHandler->handleTemplateTokens(std::move(token)));
                if (templateHandler->isTemplateEnd(token)) {
                    isInTemplateContext = false;
                }
            }

            // 2. 协程上下文
            if (isInCoroutineContext) {
                token = std::move(coroutineHandler->handleCoroutineTokens(std::move(token)));
                if (coroutineHandler->isCoroutineEnd(token)) {
                    isInCoroutineContext = false;
                }
            }

            // 3. Lambda上下文
            if (isInLambdaContext) {
                token = std::move(lambdaHandler->handleLambdaTokens(std::move(token)));
                if (lambdaHandler->isLambdaEnd(token)) {
                    isInLambdaContext = false;
                }
            }

            return std::move(token);
        }

        Token TokenLexer::peekToken() { return peekToken(1); }

        Token TokenLexer::peekToken(size_t n) {
            if (n == 0) {
                return Token(TokenKind::Invalid);
            }

            // 确保缓冲区有足够的token
            while (tokenBuffer.size() < n) {
                Token token = std::move(lexer->nextToken());

                // 处理注释
                while (commentHandler->isCommentStart(token)) {
                    commentHandler->skipComment();
                    token = std::move(lexer->nextToken());
                }

                // 处理预处理指令
                if (preprocessorHandler->isPreprocessorDirective(token)) {
                    token = std::move(preprocessorHandler->handlePreprocessorDirective());
                }

                // 检测并更新上下文状态
                updateContextState(token);

                // 处理各种上下文的token
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

            // 创建临时队列来获取第n个token
            std::queue<Token> tempBuffer;
            Token result(TokenKind::Invalid);
            size_t count = 0;

            while (!tokenBuffer.empty()) {
                count++;
                Token currentToken = std::move(tokenBuffer.front());
                if (count == n) {
                    result = currentToken;  // 复制第n个token
                }
                tempBuffer.push(std::move(currentToken));
                tokenBuffer.pop();
            }

            // 恢复token缓冲区
            tokenBuffer = std::move(tempBuffer);

            return result;
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

        // 私有辅助方法：更新上下文状态
        void TokenLexer::updateContextState(const Token& token) {
            // 检查是否进入模板上下文
            if (templateHandler->isTemplateStart(token)) {
                isInTemplateContext = true;
            }

            // 检查是否进入协程上下文
            if (coroutineHandler->isCoroutineStart(token)) {
                isInCoroutineContext = true;
            }

            // 检查是否进入Lambda上下文
            if (lambdaHandler->isLambdaStart(token)) {
                isInLambdaContext = true;
            }
        }

    }  // namespace frontend
}  // namespace rp
