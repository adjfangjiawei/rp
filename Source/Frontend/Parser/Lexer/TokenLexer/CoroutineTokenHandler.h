
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
            Token handleCoroutineTokens(const Token &current);

          private:
            std::shared_ptr<Lexer> lexer;
        };

    }  // namespace frontend
}  // namespace rp
