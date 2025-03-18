
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Binary/BinaryLiteralLexer.h"

#include <limits>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

namespace rp {
    namespace frontend {

        bool BinaryLiteralLexer::processBinaryLiteral(const std::string &input,
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
                        error = "Invalid consecutive number separators in binary literal";
                        return false;
                    }
                    if (!hasDigits) {
                        error = "Number separator cannot appear at start of binary literal";
                        return false;
                    }
                    lastWasSeparator = true;
                    hasDigitsAfterSeparator = false;
                    pos++;
                    continue;
                }

                if (!isBinaryDigit(c)) {
                    if (std::isdigit(c)) {
                        error = "Invalid binary digit '" + std::string(1, c) + "'";
                    } else {
                        error = "Invalid character in binary literal";
                    }
                    return false;
                }

                if (lastWasSeparator) {
                    hasDigitsAfterSeparator = true;
                }

                if (!checkBinaryOverflow(result, error)) {
                    return false;
                }

                result = (result << 1) | (c - '0');
                hasDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            if (!hasDigits) {
                error = "Expected binary digits after '0b'";
                return false;
            }

            if (lastWasSeparator && !hasDigitsAfterSeparator) {
                error = "Number separator cannot appear at the end of binary literal";
                return false;
            }

            if (!SuffixProcessor::processSuffix(input, pos, value, error)) {
                return false;
            }

            value.kind = NumberKind::Binary;
            value.value = result;
            return true;
        }

        bool BinaryLiteralLexer::checkBinaryOverflow(long long &result, std::string &error) {
            if (result > (std::numeric_limits<long long>::max() >> 1)) {
                error = "Binary literal is too large";
                return false;
            }
            return true;
        }

    }  // namespace frontend
}  // namespace rp
