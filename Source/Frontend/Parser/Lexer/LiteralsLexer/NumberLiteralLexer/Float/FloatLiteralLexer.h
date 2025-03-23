#pragma once

#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"

namespace rp {
    namespace frontend {

        // 前向声明
        class SuffixProcessor;

        class FloatLiteralLexer : public NumberLiteralBase {
          public:
            static bool processFloatingLiteral(const std::string &input,
                                               size_t &pos,
                                               NumberValue &value,
                                               std::string &error);

          private:
            static bool processIntegerPart(const std::string &input,
                                           size_t &pos,
                                           std::string &numStr,
                                           bool &hasDigits,
                                           bool &lastWasSeparator,
                                           bool &hasDigitsAfterSeparator,
                                           std::string &error);

            static bool processDecimalPart(const std::string &input,
                                           size_t &pos,
                                           std::string &numStr,
                                           bool &lastWasSeparator,
                                           bool &hasDigitsAfterSeparator,
                                           std::string &error);

            static bool processExponentPart(const std::string &input,
                                            size_t &pos,
                                            std::string &numStr,
                                            bool &lastWasSeparator,
                                            bool &hasDigitsAfterSeparator,
                                            std::string &error);

            static bool validateFloatRange(double value, bool isFloat, std::string &error);
        };

    }  // namespace frontend
}  // namespace rp
