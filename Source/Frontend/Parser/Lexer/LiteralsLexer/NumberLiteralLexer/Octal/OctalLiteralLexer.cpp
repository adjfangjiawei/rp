
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Octal/OctalLiteralLexer.h"

#include <limits>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

namespace rp {
    namespace frontend {

        bool OctalLiteralLexer::processOctalLiteral(const std::string &input,
                                                    size_t &pos,
                                                    NumberValue &value,
                                                    std::string &error) {
            long long result = 0;
            bool hasDigits = false;
            bool lastWasSeparator = true;  // 初始化为 true，不允许数字开头就是分隔符
            bool hasDigitsAfterSeparator = false;

            while (pos < input.length()) {
                char c = input[pos];

                if (isNumberSeparator(c)) {
                    if (lastWasSeparator) {
                        error = "Invalid consecutive number separators in octal literal";
                        return false;
                    }
                    if (!hasDigits) {
                        error = "Number separator cannot appear at start of octal literal";
                        return false;
                    }
                    lastWasSeparator = true;
                    hasDigitsAfterSeparator = false;
                    pos++;
                    continue;
                }

                if (!isOctalDigit(c)) {
                    if (std::isdigit(c)) {
                        error = "Invalid digit '" + std::string(1, c) + "' in octal literal (digits must be 0-7)";
                        return false;
                    } else {
                        // 遇到非数字字符，停止解析
                        break;
                    }
                }

                if (lastWasSeparator) {
                    hasDigitsAfterSeparator = true;
                }

                if (!checkOctalOverflow(result, error)) {
                    return false;
                }

                result = (result << 3) | (c - '0');
                hasDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            if (!hasDigits) {
                error = "Expected octal digits";
                return false;
            }

            if (lastWasSeparator && !hasDigitsAfterSeparator) {
                error = "Number separator cannot appear at the end of octal literal";
                return false;
            }

            if (!SuffixProcessor::processSuffix(input, pos, value, error)) {
                return false;
            }

            value.kind = NumberKind::Octal;
            value.value = result;
            return true;
        }

        bool OctalLiteralLexer::checkOctalOverflow(long long &result, std::string &error) {
            if (result > (std::numeric_limits<long long>::max() >> 3)) {
                error = "Octal literal is too large";
                return false;
            }
            return true;
        }

    }  // namespace frontend
}  // namespace rp
