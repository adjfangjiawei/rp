#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"

#include <cmath>
#include <limits>
#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Binary/BinaryLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Float/FloatLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Hex/HexLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Integer/IntegerLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Octal/OctalLiteralLexer.h"

namespace rp {
    namespace frontend {

        bool NumberLiteralLexer::processNumberLiteral(const std::string &input,
                                                      size_t &pos,
                                                      NumberValue &value,
                                                      std::string &error) {
            // 初始化值
            value.isUnsigned = false;
            value.isLong = false;
            value.isLongLong = false;
            value.isFloat = false;
            value.isDouble = false;
            value.value = 0.0;
            value.kind = NumberKind::Integer;

            // 验证输入不为空
            if (input.empty()) {
                error = "Empty number literal";
                return false;
            }

            // 检查第一个字符是否有效
            if (!std::isdigit(input[0]) && input[0] != '.') {
                error = "Number literal must start with a digit or decimal point";
                return false;
            }

            // 如果以小数点开始，确保后面有数字
            if (input[0] == '.' && (input.length() == 1 || !std::isdigit(input[1]))) {
                error = "Floating point literal must contain at least one digit";
                return false;
            }

            // 检查前缀并处理相应类型的字面量
            if (!checkPrefixAndProcess(input, pos, value, error)) {
                return false;
            }

            // 处理类型后缀
            if (!processTypeSuffix(input, pos, value, error)) {
                return false;
            }

            // 验证数值范围
            if (!validateValueRange(value, error)) {
                return false;
            }

            return true;
        }

        bool NumberLiteralLexer::checkPrefixAndProcess(const std::string &input,
                                                       size_t &pos,
                                                       NumberValue &value,
                                                       std::string &error) {
            // 检查前缀
            if (input[pos] == '0' && pos + 1 < input.length()) {
                char next = std::tolower(input[pos + 1]);

                // 处理十六进制
                if (next == 'x' || next == 'X') {
                    pos += 2;
                    if (pos >= input.length()) {
                        error = "Incomplete hexadecimal number literal";
                        return false;
                    }
                    if (!NumberLiteralBase::isHexDigit(input[pos])) {
                        error = "Invalid hexadecimal digit in number literal";
                        return false;
                    }
                    return HexLiteralLexer::processHexLiteral(input, pos, value, error);
                }

                // 处理二进制
                if (next == 'b' || next == 'B') {
                    pos += 2;
                    if (pos >= input.length()) {
                        error = "Incomplete binary number literal";
                        return false;
                    }
                    if (!NumberLiteralBase::isBinaryDigit(input[pos])) {
                        error = "Invalid binary digit in number literal";
                        return false;
                    }
                    return BinaryLiteralLexer::processBinaryLiteral(input, pos, value, error);
                }

                // 处理八进制
                if (next >= '0' && next <= '7') {
                    pos++;
                    return OctalLiteralLexer::processOctalLiteral(input, pos, value, error);
                }

                // 处理以0.开头的浮点数
                if (next == '.') {
                    value.kind = NumberKind::FloatingPoint;
                    return FloatLiteralLexer::processFloatingLiteral(input, pos, value, error);
                }

                // 单个0
                if (std::isspace(next) || std::ispunct(next)) {
                    value.value = 0.0;
                    value.kind = NumberKind::Integer;
                    pos++;
                    return true;
                }

                // 无效的0开头的数字
                if (std::isdigit(next)) {
                    error = "Invalid leading zero in decimal number";
                    return false;
                }
            }

            // 检查是否是浮点数
            if (isFloatingPointNumber(input, pos)) {
                value.kind = NumberKind::FloatingPoint;
                return FloatLiteralLexer::processFloatingLiteral(input, pos, value, error);
            }

            // 处理普通整数
            return IntegerLiteralLexer::processIntegerLiteral(input, pos, value, error);
        }

        bool NumberLiteralLexer::isFloatingPointNumber(const std::string &input, size_t pos) {
            bool hasDecimalPoint = false;
            bool hasExponent = false;
            bool hasDigitBeforePoint = false;
            bool hasDigitAfterPoint = false;
            bool lastWasDigit = false;
            size_t temp_pos = pos;

            // 检查数字和小数点
            while (temp_pos < input.length()) {
                char c = input[temp_pos];

                if (c == '\'') {
                    // 检查分隔符的前后是否都是数字
                    if (!lastWasDigit || temp_pos + 1 >= input.length() || !std::isdigit(input[temp_pos + 1])) {
                        return false;
                    }
                    temp_pos++;
                    continue;
                }

                if (std::isdigit(c)) {
                    if (!hasDecimalPoint) {
                        hasDigitBeforePoint = true;
                    } else {
                        hasDigitAfterPoint = true;
                    }
                    lastWasDigit = true;
                } else if (c == '.') {
                    if (hasDecimalPoint || hasExponent) return false;
                    hasDecimalPoint = true;
                    lastWasDigit = false;
                } else if (c == 'e' || c == 'E') {
                    if (hasExponent) return false;
                    if (!hasDigitBeforePoint && !hasDigitAfterPoint) return false;
                    hasExponent = true;
                    temp_pos++;

                    // 处理指数部分
                    if (temp_pos < input.length()) {
                        // 处理可选的符号
                        if (input[temp_pos] == '+' || input[temp_pos] == '-') {
                            temp_pos++;
                        }

                        // 处理指数的数字部分
                        bool hasExponentDigits = false;
                        while (temp_pos < input.length()) {
                            c = input[temp_pos];
                            if (std::isdigit(c)) {
                                hasExponentDigits = true;
                                lastWasDigit = true;
                            } else if (c == '\'') {
                                if (!lastWasDigit || temp_pos + 1 >= input.length() ||
                                    !std::isdigit(input[temp_pos + 1])) {
                                    return false;
                                }
                            } else {
                                break;
                            }
                            temp_pos++;
                        }
                        if (!hasExponentDigits) return false;
                        return true;
                    }
                    return false;
                } else {
                    break;
                }
                temp_pos++;
            }

            return (hasDecimalPoint || hasExponent) && (hasDigitBeforePoint || hasDigitAfterPoint);
        }

        bool NumberLiteralLexer::processTypeSuffix(const std::string &input,
                                                   size_t &pos,
                                                   NumberValue &value,
                                                   std::string &error) {
            while (pos < input.length()) {
                char c = input[pos];
                // 遇到空格或非后缀标点符号时停止解析
                if (std::isspace(c) ||
                    (std::ispunct(c) && c != 'u' && c != 'U' && c != 'l' && c != 'L' && c != 'f' && c != 'F')) {
                    break;
                }

                c = std::tolower(c);
                if (c == 'f') {
                    if (value.isFloat || value.isDouble) {
                        error = "Duplicate float suffix";
                        return false;
                    }
                    if (value.kind != NumberKind::FloatingPoint) {
                        value.kind = NumberKind::FloatingPoint;
                    }
                    value.isFloat = true;
                    pos++;
                } else if (c == 'l') {
                    if (value.kind == NumberKind::FloatingPoint) {
                        if (value.isFloat) {
                            error = "Invalid combination of float and long double suffix";
                            return false;
                        }
                        value.isDouble = true;
                        pos++;
                    } else {
                        if (pos + 1 < input.length() && std::tolower(input[pos + 1]) == 'l') {
                            if (value.isLong || value.isLongLong) {
                                error = "Duplicate 'll' suffix";
                                return false;
                            }
                            value.isLongLong = true;
                            pos += 2;
                        } else {
                            if (value.isLong || value.isLongLong) {
                                error = "Duplicate 'l' suffix";
                                return false;
                            }
                            value.isLong = true;
                            pos++;
                        }
                    }
                } else if (c == 'u') {
                    if (value.kind == NumberKind::FloatingPoint) {
                        error = "Invalid unsigned suffix on floating point literal";
                        return false;
                    }
                    if (value.isUnsigned) {
                        error = "Duplicate 'u' suffix";
                        return false;
                    }
                    value.isUnsigned = true;
                    pos++;
                } else {
                    error = "Invalid suffix '" + std::string(1, c) + "' on number";
                    return false;
                }
            }
            return true;
        }

        bool NumberLiteralLexer::validateValueRange(const NumberValue &value, std::string &error) {
            if (value.kind == NumberKind::FloatingPoint) {
                if (value.isFloat) {
                    if (std::abs(value.value) > std::numeric_limits<float>::max()) {
                        error = "Float literal out of range";
                        return false;
                    }
                } else {
                    if (std::abs(value.value) > std::numeric_limits<double>::max()) {
                        error = "Double literal out of range";
                        return false;
                    }
                }
            } else {
                // 整数范围检查
                double intValue = std::floor(value.value);
                if (value.isUnsigned) {
                    if (value.isLongLong) {
                        if (intValue < 0 || intValue > std::numeric_limits<unsigned long long>::max()) {
                            error = "Unsigned long long literal out of range";
                            return false;
                        }
                    } else if (value.isLong) {
                        if (intValue < 0 || intValue > std::numeric_limits<unsigned long>::max()) {
                            error = "Unsigned long literal out of range";
                            return false;
                        }
                    } else {
                        if (intValue < 0 || intValue > std::numeric_limits<unsigned int>::max()) {
                            error = "Unsigned integer literal out of range";
                            return false;
                        }
                    }
                } else {
                    if (value.isLongLong) {
                        if (intValue < std::numeric_limits<long long>::min() ||
                            intValue > std::numeric_limits<long long>::max()) {
                            error = "Long long literal out of range";
                            return false;
                        }
                    } else if (value.isLong) {
                        if (intValue < std::numeric_limits<long>::min() ||
                            intValue > std::numeric_limits<long>::max()) {
                            error = "Long literal out of range";
                            return false;
                        }
                    } else {
                        if (intValue < std::numeric_limits<int>::min() || intValue > std::numeric_limits<int>::max()) {
                            error = "Integer literal out of range";
                            return false;
                        }
                    }
                }
            }
            return true;
        }

        bool NumberLiteralLexer::validateNumberLiteral(const std::string &str, std::string &error) {
            return NumberLiteralBase::validateNumberLiteral(str, error);
        }

        Token NumberLiteralLexer::scan() {
            // 检查源代码是否有效且未结束
            if (!source || currentPos >= sourceLength) {
                return Token(TokenKind::EndOfFile, currentPos, currentLine, currentColumn);
            }

            // 检查当前字符是否可能是数字字面量的开始
            if (!std::isdigit(source[currentPos]) && source[currentPos] != '.') {
                return Token(TokenKind::Invalid, currentPos, currentLine, currentColumn);
            }

            size_t startPos = currentPos;
            size_t startLine = currentLine;
            size_t startColumn = currentColumn;

            // 构建输入字符串
            std::string input(source + currentPos);
            size_t pos = 0;
            NumberValue value;
            std::string error;

            // 处理数字字面量
            if (!processNumberLiteral(input, pos, value, error)) {
                // 报告错误
                if (diagnostics) {
                    SourceLocation loc;
                    loc.line = startLine;
                    loc.column = startColumn;
                    loc.filename = filename;
                    diagnostics->report(DiagnosticLevel::Error, loc, "Error in number literal: " + error);
                }

                // 错误恢复：至少前进一个字符，然后跳过无效字符直到找到有效的分隔符
                pos = 1;
                while (currentPos + pos < sourceLength && !std::isspace(source[currentPos + pos]) &&
                       !std::ispunct(source[currentPos + pos])) {
                    pos++;
                }

                // 更新位置信息
                for (size_t i = 0; i < pos; ++i) {
                    if (source[currentPos + i] == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                }
                currentPos += pos;

                return Token(TokenKind::Invalid, startPos, startLine, startColumn);
            }

            // 更新位置信息
            currentPos += pos;

            // 创建相应的Token
            Token token(TokenKind::NumberLiteral, startPos, startLine, startColumn);

            // 保存原始文本
            token.setText(std::string_view(source + startPos, pos));

            // 根据值的类型设置对应的token字段
            if (value.kind == NumberKind::FloatingPoint || value.isFloat || value.isDouble) {
                token.floatValue = value.value;
            } else {
                token.intValue = static_cast<long long>(value.value);
            }

            // 更新行列信息
            for (size_t i = 0; i < pos; ++i) {
                if (source[startPos + i] == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
            }

            return token;
        }

    }  // namespace frontend
}  // namespace rp
