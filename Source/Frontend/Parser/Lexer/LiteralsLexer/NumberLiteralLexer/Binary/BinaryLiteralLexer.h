#pragma once

#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"

namespace rp {
    namespace frontend {

        class BinaryLiteralLexer : public NumberLiteralBase {
          public:
            static bool processBinaryLiteral(const std::string &input,
                                             size_t &pos,
                                             NumberValue &value,
                                             std::string &error);

          private:
            static bool checkBinaryOverflow(long long &result, std::string &error);
            static bool validateValueRange(unsigned long long result, NumberValue &value, std::string &error);
        };

    }  // namespace frontend
}  // namespace rp
