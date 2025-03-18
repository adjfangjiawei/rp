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
            Token handleTemplateTokens(Token&& current);

            // 检查是否是模板开始
            bool isTemplateStart(const Token& token);

            // 检查是否是模板结束
            bool isTemplateEnd(const Token& token);

          private:
            std::shared_ptr<Lexer> lexer;
        };

    }  // namespace frontend
}  // namespace rp
