#pragma once

#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"

namespace rp {
    namespace frontend {

        // 前向声明
        class SuffixProcessor;

        class IntegerLiteralLexer : public NumberLiteralBase {
          public:
            static bool processIntegerLiteral(const std::string &input,
                                              size_t &pos,
                                              NumberValue &value,
                                              std::string &error);

          private:
            static bool checkIntegerOverflow(long long &result, int digit, std::string &error);
        };

    }  // namespace frontend
}  // namespace rp
