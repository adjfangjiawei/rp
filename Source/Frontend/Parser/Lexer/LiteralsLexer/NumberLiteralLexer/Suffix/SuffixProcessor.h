
#pragma once

#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"

namespace rp {
    namespace frontend {

        class SuffixProcessor {
          public:
            static bool processSuffix(const std::string &input, size_t &pos, NumberValue &value, std::string &error);

          private:
            enum class SuffixState { Start, AfterU, AfterL, AfterLL, AfterF, AfterD };

            static bool validateSuffixCombination(const NumberValue &value, std::string &error);
        };

    }  // namespace frontend
}  // namespace rp
