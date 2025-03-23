#pragma once

#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"

namespace rp {
    namespace frontend {

        // 前向声明
        class SuffixProcessor;

        class OctalLiteralLexer : public NumberLiteralBase {
          public:
            static bool processOctalLiteral(const std::string &input,
                                            size_t &pos,
                                            NumberValue &value,
                                            std::string &error);

          private:
            static bool checkOctalOverflow(long long &result, std::string &error);
        };

    }  // namespace frontend
}  // namespace rp
