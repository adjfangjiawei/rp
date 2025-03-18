
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
            bool lastWasSeparator = false;        // 初始化为false，因为我们还没有遇到任何字符
            bool hasDigitsAfterSeparator = true;  // 初始化为true，因为我们还没有遇到分隔符

            while (pos < input.length()) {
                char c = input[pos];

                // 处理数字分隔符
                if (isNumberSeparator(c)) {
                    // 检查是否满足分隔符的使用条件
                    if (!hasDigits) {
                        error = "Number separator cannot appear at the start";
                        return false;
                    }
                    if (lastWasSeparator) {
                        error = "Invalid consecutive number separators";
                        return false;
                    }
                    // 检查分隔符后是否还有数字
                    if (pos + 1 >= input.length()) {
                        error = "Number separator cannot appear at the end of integer literal";
                        return false;
                    }
                    if (!isDigit(input[pos + 1])) {
                        error = "Number separator must be followed by a digit";
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

            // 最终验证
            if (!hasDigits) {
                error = "Integer literal must contain at least one digit";
                return false;
            }

            if (lastWasSeparator || !hasDigitsAfterSeparator) {
                error = "Number separator cannot appear at the end of integer literal";
                return false;
            }

            // 验证分隔符的使用是否合法
            if (!validateSeparatorUsage(hasDigits, false, true, "integer literal", error)) {
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
