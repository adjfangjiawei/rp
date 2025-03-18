
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Integer/IntegerLiteralLexer.h"

#include <limits>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

namespace rp {
    namespace frontend {

        bool IntegerLiteralLexer::processIntegerLiteral(const std::string &input,
                                                        size_t &pos,
                                                        NumberValue &value,
                                                        std::string &error) {
            long long result = 0;
            bool hasDigits = false;
            bool lastWasSeparator = true;  // 不允许数字开头就是分隔符
            bool hasDigitsAfterSeparator = false;

            while (pos < input.length()) {
                char c = input[pos];

                if (isNumberSeparator(c)) {
                    if (lastWasSeparator) {
                        error = "Invalid consecutive number separators";
                        return false;
                    }
                    if (!hasDigits) {
                        error = "Number separator cannot appear at the start";
                        return false;
                    }
                    lastWasSeparator = true;
                    hasDigitsAfterSeparator = false;
                    pos++;
                    continue;
                }

                if (!isDigit(c)) {
                    break;
                }

                if (lastWasSeparator) {
                    hasDigitsAfterSeparator = true;
                }

                int digit = digitValue(c);
                if (!checkIntegerOverflow(result, digit, error)) {
                    return false;
                }

                result = result * 10 + digit;
                hasDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            if (!validateSeparatorUsage(
                    hasDigits, lastWasSeparator, hasDigitsAfterSeparator, "integer literal", error)) {
                return false;
            }

            if (!SuffixProcessor::processSuffix(input, pos, value, error)) {
                return false;
            }

            value.kind = NumberKind::Integer;
            value.value = result;
            return true;
        }

        bool IntegerLiteralLexer::checkIntegerOverflow(long long &result, int digit, std::string &error) {
            if (result > (std::numeric_limits<long long>::max() - digit) / 10) {
                error = "Integer literal is too large";
                return false;
            }
            return true;
        }

    }  // namespace frontend
}  // namespace rp
