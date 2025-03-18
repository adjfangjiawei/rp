
#pragma once

#include <cstdint>
#include <string>

namespace rp {
    namespace frontend {

        // 数字类型枚举
        enum class NumberKind { Integer, FloatingPoint, Binary, Octal, Hexadecimal };

        // 数字值结构体
        struct NumberValue {
            double value;
            NumberKind kind;
            bool isUnsigned;
            bool isLong;
            bool isLongLong;
            bool isFloat;
            bool isDouble;
        };

        class NumberLiteralBase {
          public:
            static bool isDigit(uint32_t codepoint);
            static bool isHexDigit(uint32_t codepoint);
            static bool isBinaryDigit(uint32_t codepoint);
            static bool isOctalDigit(uint32_t codepoint);
            static bool isNumberSeparator(uint32_t codepoint);
            static int digitValue(uint32_t codepoint);
            static int hexDigitValue(uint32_t codepoint);
            static bool validateNumberLiteral(const std::string &str, std::string &error);

          protected:
            static bool validateSeparatorUsage(bool hasDigits,
                                               bool lastWasSeparator,
                                               bool hasDigitsAfterSeparator,
                                               const std::string &context,
                                               std::string &error);
        };

    }  // namespace frontend
}  // namespace rp
