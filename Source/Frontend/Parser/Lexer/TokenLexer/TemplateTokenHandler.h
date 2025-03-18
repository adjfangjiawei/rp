
#pragma once
#include <memory>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class TemplateTokenHandler {
          public:
            explicit TemplateTokenHandler(std::shared_ptr<Lexer> lexer);

            // 处理模板相关的token
            Token handleTemplateTokens(const Token &current);

          private:
            std::shared_ptr<Lexer> lexer;
        };

    }  // namespace frontend
}  // namespace rp
