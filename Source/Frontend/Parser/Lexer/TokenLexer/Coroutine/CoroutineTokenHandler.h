#pragma once
#include <memory>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class CoroutineTokenHandler {
          public:
            explicit CoroutineTokenHandler(std::shared_ptr<Lexer> lexer);

            // 处理协程相关的token
            Token handleCoroutineTokens(Token&& current);

            // 检查是否是协程开始
            bool isCoroutineStart(const Token& token);

            // 检查是否是协程结束
            bool isCoroutineEnd(const Token& token);

          private:
            std::shared_ptr<Lexer> lexer;
        };

    }  // namespace frontend
}  // namespace rp
