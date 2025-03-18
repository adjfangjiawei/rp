
#include "OperatorScanner.h"

#include <cctype>
#include <sstream>

namespace rp {
    namespace frontend {

        // 初始化静态成员
        const OperatorScanner::OperatorMap OperatorScanner::singleCharOperators = initSingleCharOperators();
        const OperatorScanner::OperatorMap OperatorScanner::doubleCharOperators = initDoubleCharOperators();
        const OperatorScanner::OperatorMap OperatorScanner::tripleCharOperators = initTripleCharOperators();

        OperatorScanner::OperatorScanner(std::shared_ptr<DiagnosticEngine> diagEngine)
            : BaseScanner(std::move(diagEngine)) {}

        OperatorScanner::OperatorMap OperatorScanner::initSingleCharOperators() {
            return {{"(", TokenKind::LParen},    {")", TokenKind::RParen},  {"[", TokenKind::LSquare},
                    {"]", TokenKind::RSquare},   {"{", TokenKind::LBrace},  {"}", TokenKind::RBrace},
                    {";", TokenKind::Semicolon}, {",", TokenKind::Comma},   {"~", TokenKind::Tilde},
                    {"?", TokenKind::Question},  {"*", TokenKind::Star},    {"/", TokenKind::Slash},
                    {"%", TokenKind::Percent},   {"^", TokenKind::Caret},   {"|", TokenKind::Pipe},
                    {"&", TokenKind::Ampersand}, {"<", TokenKind::Less},    {">", TokenKind::Greater},
                    {"=", TokenKind::Equal},     {"!", TokenKind::Exclaim}, {"+", TokenKind::Plus},
                    {"-", TokenKind::Minus},     {".", TokenKind::Period},  {":", TokenKind::Colon},
                    {"#", TokenKind::Hash}};
        }

        OperatorScanner::OperatorMap OperatorScanner::initDoubleCharOperators() {
            return {{"++", TokenKind::PlusPlus},       {"+=", TokenKind::PlusEqual},    {"--", TokenKind::MinusMinus},
                    {"-=", TokenKind::MinusEqual},     {"->", TokenKind::Arrow},        {"*=", TokenKind::StarEqual},
                    {"/=", TokenKind::SlashEqual},     {"%=", TokenKind::PercentEqual}, {"^=", TokenKind::CaretEqual},
                    {"&&", TokenKind::AmpAmp},         {"&=", TokenKind::AmpEqual},     {"||", TokenKind::PipePipe},
                    {"|=", TokenKind::PipeEqual},      {"<<", TokenKind::LessLess},     {"<=", TokenKind::LessEqual},
                    {">>", TokenKind::GreaterGreater}, {">=", TokenKind::GreaterEqual}, {"==", TokenKind::EqualEqual},
                    {"!=", TokenKind::ExclaimEqual},   {"::", TokenKind::ColonColon},   {"##", TokenKind::HashHash}};
        }

        OperatorScanner::OperatorMap OperatorScanner::initTripleCharOperators() {
            return {{"...", TokenKind::Ellipsis},
                    {"<<<", TokenKind::LessLessLess},
                    {">>>", TokenKind::GreaterGreaterGreater},
                    {"<=>", TokenKind::Spaceship}};
        }

        Token OperatorScanner::scanOperatorOrPunctuation() {
            if (currentPos >= sourceLength) {
                return createToken(TokenKind::EndOfFile);
            }

            size_t startPos = currentPos;
            size_t startColumn = currentColumn;

            // 尝试匹配三字符运算符
            if (currentPos + 2 < sourceLength) {
                std::string_view tripleOp = getOperatorText(currentPos, 3);
                auto tripleIt = tripleCharOperators.find(tripleOp);
                if (tripleIt != tripleCharOperators.end()) {
                    currentPos += 3;
                    currentColumn += 3;
                    return createToken(tripleIt->second, std::string(tripleOp), startPos);
                }
            }

            // 尝试匹配双字符运算符
            if (currentPos + 1 < sourceLength) {
                std::string_view doubleOp = getOperatorText(currentPos, 2);
                auto doubleIt = doubleCharOperators.find(doubleOp);
                if (doubleIt != doubleCharOperators.end()) {
                    currentPos += 2;
                    currentColumn += 2;
                    return createToken(doubleIt->second, std::string(doubleOp), startPos);
                }
            }

            // 尝试匹配单字符运算符
            std::string_view singleOp = getOperatorText(currentPos, 1);
            auto singleIt = singleCharOperators.find(singleOp);
            if (singleIt != singleCharOperators.end()) {
                currentPos++;
                currentColumn++;
                return createToken(singleIt->second, std::string(singleOp), startPos);
            }

            // 处理无效运算符
            reportInvalidOperator(source[currentPos], startColumn);
            skipInvalidOperators();
            return createToken(TokenKind::Invalid);
        }

        bool OperatorScanner::isOperatorStart(char c) const {
            return singleCharOperators.find(std::string_view(&c, 1)) != singleCharOperators.end();
        }

        bool OperatorScanner::isValidOperatorContinuation(char c) const {
            static const std::string validContinuations = "=&|<>+-.:#+";
            return validContinuations.find(c) != std::string::npos;
        }

        std::string_view OperatorScanner::getOperatorText(size_t start, size_t length) const {
            if (start + length > sourceLength) {
                return std::string_view();
            }
            return std::string_view(source + start, length);
        }

        void OperatorScanner::reportInvalidOperator(char c, size_t column) {
            std::stringstream ss;
            ss << "Invalid operator or punctuation: '" << c << "'\n";

            // 添加可能的建议
            ss << "Valid operators are:\n";
            ss << "Single character: + - * / % & | ^ ! ~ = < > . , ; : ( ) [ ] { } ?\n";
            ss << "Double character: ++ -- += -= *= /= %= &= |= ^= << >> <= >= == != -> ::\n";
            ss << "Triple character: ... <<< >>> <=>\n";

            // 如果是一些常见的错误，添加具体建议
            if (c == '\\') {
                ss << "\nDid you mean to use '/'?\n";
            } else if (c == '\'') {
                ss << "\nSingle quotes should be straight (') not curly (')\n";
            } else if (c == '"' || c == '"') {
                ss << "\nDouble quotes should be straight (\") not curly (\")\n";
            }

            reportError(ss.str());
        }

        void OperatorScanner::skipInvalidOperators() {
            size_t skippedCount = 0;
            std::string skippedChars;

            while (currentPos < sourceLength && skippedCount < 10) {
                char c = source[currentPos];

                // 如果遇到明显的分隔符或空白字符，停止跳过
                if (std::isspace(c) || c == ';' || c == '\n') {
                    break;
                }

                // 如果遇到有效的运算符起始字符，也停止跳过
                if (isOperatorStart(c) && !skippedChars.empty()) {
                    break;
                }

                skippedChars += c;
                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
                currentPos++;
                skippedCount++;
            }

            // 如果跳过了字符，报告警告
            if (!skippedChars.empty()) {
                std::stringstream ss;
                ss << "Skipped invalid characters: '" << skippedChars << "'";
                reportWarning(ss.str());
            }
        }

    }  // namespace frontend
}  // namespace rp
