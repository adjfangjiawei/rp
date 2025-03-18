
#pragma once
#include <memory>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class CommentHandler {
          public:
            explicit CommentHandler(std::shared_ptr<Lexer> lexer);

            void skipComment();
            bool isCommentStart(const Token &token) const;

          private:
            void skipLineComment();
            void skipBlockComment();

            std::shared_ptr<Lexer> lexer;
        };

    }  // namespace frontend
}  // namespace rp
