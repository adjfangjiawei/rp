
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Float/FloatLiteralLexer.h"

#include <cmath>
#include <limits>
#include <stdexcept>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

namespace rp {
    namespace frontend {

        bool FloatLiteralLexer::processFloatingLiteral(const std::string &input,
                                                       size_t &pos,
                                                       NumberValue &value,
                                                       std::string &error) {
            std::string numStr;
            bool hasDigits = false;
            bool lastWasSeparator = true;
            bool hasDigitsAfterSeparator = false;

            // 处理整数部分
            if (!processIntegerPart(input, pos, numStr, hasDigits, lastWasSeparator, hasDigitsAfterSeparator, error)) {
                return false;
            }

            // 处理小数点和小数部分
            if (pos < input.length() && input[pos] == '.') {
                if (lastWasSeparator) {
                    error = "Number separator cannot appear before decimal point";
                    return false;
                }

                numStr += input[pos++];
                lastWasSeparator = false;

                if (!processDecimalPart(input, pos, numStr, lastWasSeparator, hasDigitsAfterSeparator, error)) {
                    return false;
                }
            }

            // 处理指数部分
            if (pos < input.length() && (input[pos] == 'e' || input[pos] == 'E')) {
                if (lastWasSeparator) {
                    error = "Number separator cannot appear before exponent";
                    return false;
                }

                if (!processExponentPart(input, pos, numStr, lastWasSeparator, hasDigitsAfterSeparator, error)) {
                    return false;
                }
            }

            if (!hasDigits) {
                error = "Expected digits in floating literal";
                return false;
            }

            if (!SuffixProcessor::processSuffix(input, pos, value, error)) {
                return false;
            }

            value.kind = NumberKind::FloatingPoint;

            try {
                double doubleValue = std::stod(numStr);
                if (!validateFloatRange(doubleValue, value.isFloat, error)) {
                    return false;
                }
                value.value = doubleValue;
                return true;
            } catch (const std::out_of_range &) {
                error = "Floating point literal is out of range";
                return false;
            } catch (const std::invalid_argument &) {
                error = "Invalid floating point literal format";
                return false;
            }
        }

        bool FloatLiteralLexer::processIntegerPart(const std::string &input,
                                                   size_t &pos,
                                                   std::string &numStr,
                                                   bool &hasDigits,
                                                   bool &lastWasSeparator,
                                                   bool &hasDigitsAfterSeparator,
                                                   std::string &error) {
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

                numStr += c;
                hasDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            return true;
        }

        bool FloatLiteralLexer::processDecimalPart(const std::string &input,
                                                   size_t &pos,
                                                   std::string &numStr,
                                                   bool &lastWasSeparator,
                                                   bool &hasDigitsAfterSeparator,
                                                   std::string &error) {
            bool hasDecimalDigits = false;

            while (pos < input.length()) {
                char c = input[pos];

                if (isNumberSeparator(c)) {
                    if (lastWasSeparator) {
                        error = "Invalid consecutive number separators";
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

                numStr += c;
                hasDecimalDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            if (!hasDecimalDigits) {
                error = "Expected digits after decimal point";
                return false;
            }

            return true;
        }

        bool FloatLiteralLexer::processExponentPart(const std::string &input,
                                                    size_t &pos,
                                                    std::string &numStr,
                                                    bool &lastWasSeparator,
                                                    bool &hasDigitsAfterSeparator,
                                                    std::string &error) {
            numStr += input[pos++];

            // 处理指数符号
            if (pos < input.length() && (input[pos] == '+' || input[pos] == '-')) {
                numStr += input[pos++];
            }

            lastWasSeparator = false;
            bool hasExponentDigits = false;

            while (pos < input.length()) {
                char c = input[pos];

                if (isNumberSeparator(c)) {
                    if (lastWasSeparator) {
                        error = "Invalid consecutive number separators in exponent";
                        return false;
                    }
                    if (!hasExponentDigits) {
                        error = "Number separator cannot appear at start of exponent";
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

                numStr += c;
                hasExponentDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            if (!hasExponentDigits) {
                error = "Expected digits after exponent";
                return false;
            }

            return true;
        }

        bool FloatLiteralLexer::validateFloatRange(double value, bool isFloat, std::string &error) {
            if (isFloat) {
                if (std::abs(value) > std::numeric_limits<float>::max()) {
                    error = "Float literal is too large";
                    return false;
                }
                if (value != 0.0 && std::abs(value) < std::numeric_limits<float>::min()) {
                    error = "Float literal is too small";
                    return false;
                }
            } else {
                if (std::isinf(value)) {
                    error = "Double literal is too large";
                    return false;
                }
                if (value != 0.0 && std::abs(value) < std::numeric_limits<double>::min()) {
                    error = "Double literal is too small";
                    return false;
                }
            }
            return true;
        }

    }  // namespace frontend
}  // namespace rp
