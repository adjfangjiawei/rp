
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Hex/HexLiteralLexer.h"

#include <cmath>
#include <limits>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

namespace rp {
    namespace frontend {

        bool HexLiteralLexer::processHexLiteral(const std::string &input,
                                                size_t &pos,
                                                NumberValue &value,
                                                std::string &error) {
            long long result = 0;
            double fraction = 0.0;
            bool hasExponent = false;
            int exponent = 0;
            bool hasDot = false;
            bool hasDigits = false;
            bool lastWasSeparator = true;  // 不允许数字开头就是分隔符
            bool hasDigitsAfterSeparator = false;

            while (pos < input.length()) {
                char c = input[pos];

                if (isNumberSeparator(c)) {
                    if (lastWasSeparator) {
                        error = "Invalid consecutive number separators in hexadecimal literal";
                        return false;
                    }
                    if (!hasDigits) {
                        error = "Number separator cannot appear at start of hexadecimal literal";
                        return false;
                    }
                    lastWasSeparator = true;
                    hasDigitsAfterSeparator = false;
                    pos++;
                    continue;
                }

                if (c == '.') {
                    if (hasDot) {
                        error = "Multiple decimal points in hexadecimal literal";
                        return false;
                    }
                    hasDot = true;
                    pos++;
                    continue;
                }

                if (c == 'p' || c == 'P') {
                    if (hasExponent) {
                        error = "Multiple exponents in hexadecimal literal";
                        return false;
                    }
                    pos++;
                    if (pos >= input.length()) {
                        error = "Expected exponent value after 'p'";
                        return false;
                    }

                    // 处理指数的符号
                    bool negativeExponent = false;
                    if (input[pos] == '+' || input[pos] == '-') {
                        negativeExponent = (input[pos] == '-');
                        pos++;
                    }

                    // 读取指数值
                    if (pos >= input.length() || !std::isdigit(input[pos])) {
                        error = "Expected decimal digits after exponent";
                        return false;
                    }

                    while (pos < input.length() && std::isdigit(input[pos])) {
                        exponent = exponent * 10 + (input[pos] - '0');
                        pos++;
                    }

                    if (negativeExponent) {
                        exponent = -exponent;
                    }

                    hasExponent = true;
                    continue;
                }

                if (!isHexDigit(c)) {
                    break;
                }

                if (lastWasSeparator) {
                    hasDigitsAfterSeparator = true;
                }

                int digitVal = hexDigitValue(c);
                if (digitVal == -1) {
                    error = "Invalid hexadecimal digit";
                    return false;
                }

                if (hasDot) {
                    // 处理小数部分
                    fraction = fraction / 16.0 + (digitVal / 16.0);
                } else {
                    // 处理整数部分
                    if (!checkHexOverflow(result, error)) {
                        return false;
                    }
                    result = (result << 4) | digitVal;
                }

                hasDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            if (!hasDigits) {
                error = "Expected hexadecimal digits after '0x'";
                return false;
            }

            if (lastWasSeparator && !hasDigitsAfterSeparator) {
                error = "Number separator cannot appear at the end of hexadecimal literal";
                return false;
            }

            // 计算最终值
            double finalValue = result + fraction;
            if (hasExponent) {
                finalValue *= std::pow(2.0, exponent);
            }

            if (!SuffixProcessor::processSuffix(input, pos, value, error)) {
                return false;
            }

            value.kind = hasDot || hasExponent ? NumberKind::FloatingPoint : NumberKind::Hexadecimal;
            value.value = finalValue;
            return true;
        }

        bool HexLiteralLexer::checkHexOverflow(long long &result, std::string &error) {
            if (result > (std::numeric_limits<long long>::max() >> 4)) {
                error = "Hexadecimal literal is too large";
                return false;
            }
            return true;
        }

    }  // namespace frontend
}  // namespace rp
