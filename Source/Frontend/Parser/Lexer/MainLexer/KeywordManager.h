
#pragma once

#include <string>
#include <unordered_map>

#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class KeywordManager {
          public:
            static void initialize();
            static bool isKeyword(const std::string& text, TokenKind& kind);

          private:
            static std::unordered_map<std::string, TokenKind> keywords;
            static void initializeKeywordMap();
        };

    }  // namespace frontend
}  // namespace rp
