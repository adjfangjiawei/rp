
#pragma once
#include <memory>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class LambdaTokenHandler {
          public:
            explicit LambdaTokenHandler(std::shared_ptr<Lexer> lexer);

            // 处理Lambda相关的token
            Token handleLambdaTokens(const Token &current);

          private:
            std::shared_ptr<Lexer> lexer;
        };

    }  // namespace frontend
}  // namespace rp
