#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Binary/BinaryLiteralLexer.h"

#include <bitset>
#include <limits>
#include <sstream>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

namespace rp {
    namespace frontend {

        bool BinaryLiteralLexer::processBinaryLiteral(const std::string &input,
                                                      size_t &pos,
                                                      NumberValue &value,
                                                      std::string &error) {
            unsigned long long result = 0;
            bool hasDigits = false;
            bool lastWasSeparator = false;
            bool hasDigitsAfterSeparator = false;
            size_t digitCount = 0;
            std::stringstream binaryStr;

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
                    // 如果不是二进制数字，可能是后缀或无效字符
                    break;
                }

                if (lastWasSeparator) {
                    hasDigitsAfterSeparator = true;
                }

                // 检查数字位数限制
                if (digitCount >= 64) {
                    error = "Binary literal exceeds 64 bits";
                    return false;
                }

                // 更新结果
                result = (result << 1) | (c - '0');
                binaryStr << c;
                digitCount++;
                hasDigits = true;
                lastWasSeparator = false;
                pos++;
            }

            // 验证基本要求
            if (!hasDigits) {
                error = "Expected binary digits after '0b'";
                return false;
            }

            if (lastWasSeparator && !hasDigitsAfterSeparator) {
                error = "Number separator cannot appear at the end of binary literal";
                return false;
            }

            // 处理后缀并确定具体类型
            if (!SuffixProcessor::processSuffix(input, pos, value, error)) {
                return false;
            }

            // 根据后缀验证范围并设置最终值
            if (!validateValueRange(result, value, error)) {
                return false;
            }

            // 设置数字类型和值
            value.kind = NumberKind::Binary;
            value.value = static_cast<double>(result);

            // 为错误恢复和诊断添加额外信息
            if (digitCount == 0) {
                error = "Empty binary literal";
                return false;
            }

            return true;
        }

        bool BinaryLiteralLexer::validateValueRange(unsigned long long result, NumberValue &value, std::string &error) {
            // 检查是否需要无符号类型
            bool needsUnsigned = false;
            if (result > static_cast<unsigned long long>(std::numeric_limits<long long>::max())) {
                needsUnsigned = true;
            }

            // 如果指定了有符号类型但需要无符号类型
            if (!value.isUnsigned && needsUnsigned) {
                error = "Binary literal too large for signed type";
                return false;
            }

            // 根据指定的类型检查范围
            if (value.isLongLong) {
                if (value.isUnsigned) {
                    // unsigned long long - 已经是最大范围，不需要检查
                } else {
                    if (result > static_cast<unsigned long long>(std::numeric_limits<long long>::max())) {
                        error = "Binary literal too large for signed long long";
                        return false;
                    }
                }
            } else if (value.isLong) {
                if (value.isUnsigned) {
                    if (result > std::numeric_limits<unsigned long>::max()) {
                        error = "Binary literal too large for unsigned long";
                        return false;
                    }
                } else {
                    if (result > static_cast<unsigned long>(std::numeric_limits<long>::max())) {
                        error = "Binary literal too large for signed long";
                        return false;
                    }
                }
            } else {
                // 默认 int 类型
                if (value.isUnsigned) {
                    if (result > std::numeric_limits<unsigned int>::max()) {
                        error = "Binary literal too large for unsigned int";
                        return false;
                    }
                } else {
                    if (result > static_cast<unsigned int>(std::numeric_limits<int>::max())) {
                        error = "Binary literal too large for signed int";
                        return false;
                    }
                }
            }

            // 如果没有指定类型修饰符，根据值的大小自动选择合适的类型
            if (!value.isUnsigned && !value.isLong && !value.isLongLong) {
                if (result <= static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
                    // 适合 int
                } else if (result <= static_cast<unsigned long long>(std::numeric_limits<long>::max())) {
                    value.isLong = true;
                } else if (result <= static_cast<unsigned long long>(std::numeric_limits<long long>::max())) {
                    value.isLongLong = true;
                } else {
                    value.isUnsigned = true;
                    value.isLongLong = true;
                }
            }

            return true;
        }

        bool BinaryLiteralLexer::checkBinaryOverflow(long long &result, std::string &error) {
            if (result > (std::numeric_limits<long long>::max() >> 1)) {
                std::stringstream ss;
                ss << "Binary literal exceeds maximum value of "
                   << std::bitset<64>(std::numeric_limits<long long>::max()).to_string();
                error = ss.str();
                return false;
            }
            return true;
        }

    }  // namespace frontend
}  // namespace rp
