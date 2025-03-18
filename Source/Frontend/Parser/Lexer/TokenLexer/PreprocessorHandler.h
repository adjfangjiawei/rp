
#pragma once
#include <memory>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class PreprocessorHandler {
          public:
            explicit PreprocessorHandler(std::shared_ptr<Lexer> lexer);

            // 处理预处理器指令
            Token handlePreprocessorDirective();

            // 检查是否是预处理器指令
            bool isPreprocessorDirective(const Token &token) const;

          private:
            std::shared_ptr<Lexer> lexer;
        };

    }  // namespace frontend
}  // namespace rp
