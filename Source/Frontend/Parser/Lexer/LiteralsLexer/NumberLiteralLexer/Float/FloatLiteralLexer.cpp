#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Float/FloatLiteralLexer.h"

#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
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
            bool lastWasSeparator = false;
            bool hasDigitsAfterSeparator = false;
            bool isHexFloat = false;

            // 检查是否是十六进制浮点数
            if (pos + 2 < input.length() && input[pos] == '0' && (input[pos + 1] == 'x' || input[pos + 1] == 'X')) {
                isHexFloat = true;
                pos += 2;
                numStr = "0x";
            }

            // 检查特殊值
            if (!isHexFloat && pos + 3 < input.length()) {
                std::string special = input.substr(pos, 3);
                if (special == "inf" || special == "Inf") {
                    pos += 3;
                    if (pos + 5 <= input.length() && input.substr(pos, 5) == "inity") {
                        pos += 5;
                    }
                    value.value = std::numeric_limits<double>::infinity();
                    value.kind = NumberKind::FloatingPoint;
                    return true;
                } else if (special == "nan" || special == "NaN") {
                    pos += 3;
                    value.value = std::numeric_limits<double>::quiet_NaN();
                    value.kind = NumberKind::FloatingPoint;
                    return true;
                }
            }

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
            if (pos < input.length()) {
                char expChar = input[pos];
                if ((!isHexFloat && (expChar == 'e' || expChar == 'E')) ||
                    (isHexFloat && (expChar == 'p' || expChar == 'P'))) {
                    if (lastWasSeparator) {
                        error = "Number separator cannot appear before exponent";
                        return false;
                    }

                    if (!processExponentPart(input, pos, numStr, lastWasSeparator, hasDigitsAfterSeparator, error)) {
                        return false;
                    }
                }
            }

            if (!hasDigits) {
                error = "Expected digits in floating literal";
                return false;
            }

            // 处理后缀
            if (!SuffixProcessor::processSuffix(input, pos, value, error)) {
                return false;
            }

            value.kind = NumberKind::FloatingPoint;

            try {
                // 使用高精度转换
                std::istringstream iss(numStr);
                if (isHexFloat) {
                    // 十六进制浮点数需要特殊处理
                    unsigned long long intPart = 0;
                    double fracPart = 0.0;
                    int exponent = 0;

                    // 解析十六进制字符串
                    size_t pointPos = numStr.find('.');
                    size_t expPos = numStr.find_first_of("pP");

                    // 处理整数部分
                    std::string intStr = (pointPos != std::string::npos) ? numStr.substr(2, pointPos - 2)
                                         : (expPos != std::string::npos) ? numStr.substr(2, expPos - 2)
                                                                         : numStr.substr(2);

                    if (!intStr.empty()) {
                        intPart = std::stoull(intStr, nullptr, 16);
                    }

                    // 处理小数部分
                    if (pointPos != std::string::npos) {
                        std::string fracStr = (expPos != std::string::npos)
                                                  ? numStr.substr(pointPos + 1, expPos - pointPos - 1)
                                                  : numStr.substr(pointPos + 1);

                        if (!fracStr.empty()) {
                            double scale = 1.0;
                            for (char c : fracStr) {
                                scale *= 16.0;
                                fracPart += (std::isdigit(c) ? c - '0' : std::tolower(c) - 'a' + 10) / scale;
                            }
                        }
                    }

                    // 处理指数部分
                    if (expPos != std::string::npos) {
                        std::string expStr = numStr.substr(expPos + 1);
                        exponent = std::stoi(expStr);
                    }

                    // 计算最终值
                    double result = (intPart + fracPart) * std::pow(2.0, exponent);

                    if (!validateFloatRange(result, value.isFloat, error)) {
                        return false;
                    }
                    value.value = result;
                } else {
                    // 普通浮点数转换
                    double result;
                    iss >> std::setprecision(std::numeric_limits<double>::max_digits10) >> result;

                    if (!validateFloatRange(result, value.isFloat, error)) {
                        return false;
                    }
                    value.value = result;
                }
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
            bool isHex = !numStr.empty() && numStr[0] == '0' && (numStr[1] == 'x' || numStr[1] == 'X');

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

                if (isHex ? !isHexDigit(c) : !isDigit(c)) {
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
            bool isHex = numStr.length() >= 2 && numStr[0] == '0' && (numStr[1] == 'x' || numStr[1] == 'X');

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

                if (isHex ? !isHexDigit(c) : !isDigit(c)) {
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
            char expChar = input[pos];
            numStr += expChar;
            pos++;

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
            if (std::isnan(value) || std::isinf(value)) {
                return true;  // 特殊值总是有效的
            }

            if (isFloat) {
                if (std::abs(value) > std::numeric_limits<float>::max()) {
                    error = "Float literal is too large";
                    return false;
                }
                if (value != 0.0 && std::abs(value) < std::numeric_limits<float>::min()) {
                    error = "Float literal is too small (underflow)";
                    return false;
                }
            } else {
                if (std::abs(value) > std::numeric_limits<double>::max()) {
                    error = "Double literal is too large";
                    return false;
                }
                if (value != 0.0 && std::abs(value) < std::numeric_limits<double>::min()) {
                    error = "Double literal is too small (underflow)";
                    return false;
                }
            }

            // 检查是否是非规格化数
            if (isFloat) {
                float f = static_cast<float>(value);
                if (value != 0.0 && std::abs(f) < std::numeric_limits<float>::min()) {
                    error = "Float literal becomes denormalized";
                    return false;
                }
            } else {
                if (value != 0.0 && std::abs(value) < std::numeric_limits<double>::min()) {
                    error = "Double literal becomes denormalized";
                    return false;
                }
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
