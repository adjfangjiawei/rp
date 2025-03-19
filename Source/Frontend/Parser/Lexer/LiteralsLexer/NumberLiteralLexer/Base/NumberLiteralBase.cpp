
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"

namespace rp {
    namespace frontend {

        bool NumberLiteralBase::isDigit(uint32_t codepoint) { return codepoint >= '0' && codepoint <= '9'; }

        bool NumberLiteralBase::isHexDigit(uint32_t codepoint) {
            return (codepoint >= '0' && codepoint <= '9') || (codepoint >= 'a' && codepoint <= 'f') ||
                   (codepoint >= 'A' && codepoint <= 'F');
        }

        bool NumberLiteralBase::isBinaryDigit(uint32_t codepoint) { return codepoint == '0' || codepoint == '1'; }

        bool NumberLiteralBase::isOctalDigit(uint32_t codepoint) { return codepoint >= '0' && codepoint <= '7'; }

        bool NumberLiteralBase::isNumberSeparator(uint32_t codepoint) { return codepoint == '\''; }

        int NumberLiteralBase::digitValue(uint32_t codepoint) {
            if (codepoint >= '0' && codepoint <= '9') {
                return codepoint - '0';
            }
            return -1;
        }

        int NumberLiteralBase::hexDigitValue(uint32_t codepoint) {
            if (codepoint >= '0' && codepoint <= '9') {
                return codepoint - '0';
            }
            if (codepoint >= 'a' && codepoint <= 'f') {
                return codepoint - 'a' + 10;
            }
            if (codepoint >= 'A' && codepoint <= 'F') {
                return codepoint - 'A' + 10;
            }
            return -1;
        }

        bool NumberLiteralBase::validateNumberLiteral(const std::string &str, std::string &error) {
            if (str.empty()) {
                error = "Empty number literal";
                return false;
            }

            // 检查基本格式
            if (!isDigit(str[0]) && str[0] != '.') {
                error = "Invalid start of number literal";
                return false;
            }

            // 检查是否有多个小数点
            int decimalPoints = 0;
            for (char c : str) {
                if (c == '.') decimalPoints++;
            }
            if (decimalPoints > 1) {
                error = "Multiple decimal points in number literal";
                return false;
            }

            return true;
        }

        bool NumberLiteralBase::validateSeparatorUsage(bool hasDigits,
                                                       bool lastWasSeparator,
                                                       bool hasDigitsAfterSeparator,
                                                       const std::string &context,
                                                       std::string &error) {
            if (lastWasSeparator && !hasDigitsAfterSeparator) {
                error = "Number separator cannot appear at the end of " + context;
                return false;
            }

            if (!hasDigits) {
                error = "Expected digits in " + context;
                return false;
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
