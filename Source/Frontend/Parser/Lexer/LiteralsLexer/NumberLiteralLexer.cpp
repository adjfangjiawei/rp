
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"

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

            // 检查前缀并处理相应类型的字面量
            if (!checkPrefixAndProcess(input, pos, value, error)) {
                return false;
            }

            return true;
        }

        bool NumberLiteralLexer::checkPrefixAndProcess(const std::string &input,
                                                       size_t &pos,
                                                       NumberValue &value,
                                                       std::string &error) {
            // 检查前缀
            if (input[pos] == '0') {
                if (pos + 1 < input.length()) {
                    char next = std::tolower(input[pos + 1]);
                    if (next == 'x' || next == 'X') {
                        pos += 2;
                        return HexLiteralLexer::processHexLiteral(input, pos, value, error);
                    } else if (next == 'b' || next == 'B') {
                        pos += 2;
                        return BinaryLiteralLexer::processBinaryLiteral(input, pos, value, error);
                    } else if (std::isdigit(next)) {
                        pos++;
                        return OctalLiteralLexer::processOctalLiteral(input, pos, value, error);
                    }
                }
            }

            // 检查是否是浮点数
            if (isFloatingPointNumber(input, pos)) {
                return FloatLiteralLexer::processFloatingLiteral(input, pos, value, error);
            }

            // 处理普通整数
            return IntegerLiteralLexer::processIntegerLiteral(input, pos, value, error);
        }

        bool NumberLiteralLexer::isFloatingPointNumber(const std::string &input, size_t pos) {
            bool hasDecimalPoint = false;
            bool hasExponent = false;
            size_t temp_pos = pos;

            while (temp_pos < input.length()) {
                char c = input[temp_pos];
                if (c == '.') hasDecimalPoint = true;
                if (c == 'e' || c == 'E') hasExponent = true;
                if (hasDecimalPoint || hasExponent) break;
                temp_pos++;
            }

            return hasDecimalPoint || hasExponent;
        }

        bool NumberLiteralLexer::validateNumberLiteral(const std::string &str, std::string &error) {
            return NumberLiteralBase::validateNumberLiteral(str, error);
        }

        Token NumberLiteralLexer::scan() {
            if (!source || currentPos >= sourceLength) {
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
                // TODO: 报告错误
                if (diagnostics) {
                    SourceLocation loc;
                    loc.line = startLine;
                    loc.column = startColumn;
                    diagnostics->report(DiagnosticLevel::Error, loc, error);
                }
                currentPos += pos;
                return Token(TokenKind::Invalid, startPos, startLine, startColumn);
            }

            // 更新位置信息
            currentPos += pos;

            // 创建相应的Token
            TokenKind kind;
            if (value.kind == NumberKind::Integer) {
                kind = TokenKind::NumberLiteral;
            } else if (value.kind == NumberKind::FloatingPoint) {
                kind = TokenKind::NumberLiteral;
            } else {
                kind = TokenKind::Invalid;
            }

            return Token(kind, startPos, startLine, startColumn, value);
        }

    }  // namespace frontend
}  // namespace rp
