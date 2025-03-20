
#include "Scanner.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <sstream>

namespace rp {
    namespace frontend {

        Scanner::Scanner(std::shared_ptr<DiagnosticEngine> diagEngine)
            : identifierScanner(diagEngine), operatorScanner(diagEngine), diagnostics(std::move(diagEngine)) {
            if (!diagnostics) {
                throw std::runtime_error("DiagnosticEngine cannot be null");
            }
        }

        void Scanner::setSource(const char* src, size_t length, const std::string& filename) {
            if (!src && length > 0) {
                throw std::invalid_argument("Source pointer cannot be null when length > 0");
            }

            source = src;
            sourceLength = length;
            this->filename = filename;

            identifierScanner.setSource(src, length, filename);
            operatorScanner.setSource(src, length, filename);
        }

        Token Scanner::scanIdentifier() {
            if (isAtEnd() || !isIdentifierStart(getCurrentChar())) {
                return Token(TokenKind::Invalid);
            }

            Token result = identifierScanner.scanIdentifier();
            syncScannerPositions();
            return result;
        }

        Token Scanner::scanNumber() {
            if (isAtEnd() || !isDigit(getCurrentChar())) {
                return Token(TokenKind::Invalid);
            }

            size_t start = getCurrentPos();
            bool isHex = false;
            bool isOctal = false;
            bool isFloat = false;
            bool hasExponent = false;

            // 检查前缀
            if (getCurrentChar() == '0') {
                if (auto next = peekChar(1)) {
                    if (*next == 'x' || *next == 'X') {
                        isHex = true;
                        updatePosition(getCurrentPos() + 2);
                    } else if (*next >= '0' && *next <= '7') {
                        isOctal = true;
                        updatePosition(getCurrentPos() + 1);
                    }
                }
            }

            // 扫描数字部分
            while (!isAtEnd()) {
                char c = getCurrentChar();
                if (isHex && isHexDigit(c)) {
                    updatePosition(getCurrentPos() + 1);
                } else if (isOctal && isOctalDigit(c)) {
                    updatePosition(getCurrentPos() + 1);
                } else if (!isHex && !isOctal && (isDigit(c) || c == '.' || c == 'e' || c == 'E')) {
                    if (c == '.') {
                        if (isFloat) {
                            reportError("Multiple decimal points in number");
                            return Token(TokenKind::Invalid);
                        }
                        isFloat = true;
                    } else if (c == 'e' || c == 'E') {
                        if (hasExponent) {
                            reportError("Multiple exponents in number");
                            return Token(TokenKind::Invalid);
                        }
                        hasExponent = true;
                        updatePosition(getCurrentPos() + 1);
                        if (!isAtEnd() && (getCurrentChar() == '+' || getCurrentChar() == '-')) {
                            updatePosition(getCurrentPos() + 1);
                        }
                        continue;
                    }
                    updatePosition(getCurrentPos() + 1);
                } else {
                    break;
                }
            }

            // 处理后缀
            if (!isAtEnd()) {
                char c = getCurrentChar();
                if (c == 'f' || c == 'F' || c == 'l' || c == 'L' || c == 'u' || c == 'U') {
                    updatePosition(getCurrentPos() + 1);
                }
            }

            std::string number(source + start, getCurrentPos() - start);
            Token token(TokenKind::NumberLiteral);
            token.setText(number);

            // 尝试转换数值
            try {
                if (isFloat || hasExponent) {
                    token.floatValue = std::stod(number);
                } else if (isHex) {
                    token.intValue = std::stoll(number, nullptr, 16);
                } else if (isOctal) {
                    token.intValue = std::stoll(number, nullptr, 8);
                } else {
                    token.intValue = std::stoll(number);
                }
            } catch (const std::exception& e) {
                reportError("Invalid number format: " + std::string(e.what()));
                return Token(TokenKind::Invalid);
            }

            return token;
        }

        Token Scanner::scanString() {
            if (isAtEnd() || getCurrentChar() != '"') {
                return Token(TokenKind::Invalid);
            }

            size_t start = getCurrentPos();
            updatePosition(getCurrentPos() + 1);  // 跳过开始的引号

            std::string value;
            bool escaped = false;

            while (!isAtEnd()) {
                char c = getCurrentChar();

                if (escaped) {
                    switch (c) {
                        case 'n':
                            value += '\n';
                            break;
                        case 't':
                            value += '\t';
                            break;
                        case 'r':
                            value += '\r';
                            break;
                        case '\\':
                            value += '\\';
                            break;
                        case '"':
                            value += '"';
                            break;
                        case '0':
                            value += '\0';
                            break;
                        default:
                            reportWarning("Unknown escape sequence '\\" + std::string(1, c) + "'");
                            value += c;
                    }
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == '"') {
                    updatePosition(getCurrentPos() + 1);  // 跳过结束的引号
                    Token token(TokenKind::StringLiteral);
                    token.setText(value);
                    return token;
                } else if (c == '\n') {
                    reportError("Unterminated string literal");
                    return Token(TokenKind::Invalid);
                } else {
                    value += c;
                }

                updatePosition(getCurrentPos() + 1);
            }

            reportError("Unterminated string literal");
            return Token(TokenKind::Invalid);
        }

        Token Scanner::scanCharacter() {
            if (isAtEnd() || getCurrentChar() != '\'') {
                return Token(TokenKind::Invalid);
            }

            size_t start = getCurrentPos();
            updatePosition(getCurrentPos() + 1);  // 跳过开始的引号

            if (isAtEnd()) {
                reportError("Empty character literal");
                return Token(TokenKind::Invalid);
            }

            char value;
            if (getCurrentChar() == '\\') {
                updatePosition(getCurrentPos() + 1);
                if (isAtEnd()) {
                    reportError("Incomplete escape sequence");
                    return Token(TokenKind::Invalid);
                }

                switch (getCurrentChar()) {
                    case 'n':
                        value = '\n';
                        break;
                    case 't':
                        value = '\t';
                        break;
                    case 'r':
                        value = '\r';
                        break;
                    case '\\':
                        value = '\\';
                        break;
                    case '\'':
                        value = '\'';
                        break;
                    case '0':
                        value = '\0';
                        break;
                    default:
                        reportError("Invalid escape sequence");
                        return Token(TokenKind::Invalid);
                }
                updatePosition(getCurrentPos() + 1);
            } else {
                value = getCurrentChar();
                updatePosition(getCurrentPos() + 1);
            }

            if (isAtEnd() || getCurrentChar() != '\'') {
                reportError("Unterminated character literal");
                return Token(TokenKind::Invalid);
            }

            updatePosition(getCurrentPos() + 1);  // 跳过结束的引号

            Token token(TokenKind::CharLiteral);
            token.setText(std::string(1, value));
            token.intValue = static_cast<long long>(value);
            return token;
        }

        Token Scanner::scanComment() {
            if (isAtEnd() || getCurrentChar() != '/') {
                return Token(TokenKind::Invalid);
            }

            auto next = peekChar(1);
            if (!next) {
                return Token(TokenKind::Invalid);
            }

            if (*next == '/') {
                // 单行注释
                skipUntilNewline();
                return scanOperatorOrPunctuation();  // 继续扫描下一个token
            } else if (*next == '*') {
                // 多行注释
                updatePosition(getCurrentPos() + 2);  // 跳过 /*
                while (!isAtEnd()) {
                    if (getCurrentChar() == '*' && peekChar(1) && *peekChar(1) == '/') {
                        updatePosition(getCurrentPos() + 2);  // 跳过 */
                        return scanOperatorOrPunctuation();   // 继续扫描下一个token
                    }
                    updatePosition(getCurrentPos() + 1);
                }
                reportError("Unterminated multi-line comment");
            }

            return Token(TokenKind::Invalid);
        }

        Token Scanner::scanPreprocessor() {
            if (isAtEnd() || getCurrentChar() != '#') {
                return Token(TokenKind::Invalid);
            }

            size_t start = getCurrentPos();
            updatePosition(getCurrentPos() + 1);  // 跳过 #

            // 跳过空白字符
            while (!isAtEnd() && isWhitespace(getCurrentChar()) && getCurrentChar() != '\n') {
                updatePosition(getCurrentPos() + 1);
            }

            // 读取指令名称
            std::string directive;
            while (!isAtEnd() && isIdentifierContinue(getCurrentChar())) {
                directive += getCurrentChar();
                updatePosition(getCurrentPos() + 1);
            }

            // 根据指令类型返回对应的Token
            TokenKind kind;
            if (directive == "include")
                kind = TokenKind::Directive_Include;
            else if (directive == "define")
                kind = TokenKind::Directive_Define;
            else if (directive == "ifdef")
                kind = TokenKind::Directive_Ifdef;
            else if (directive == "ifndef")
                kind = TokenKind::Directive_Ifndef;
            else if (directive == "endif")
                kind = TokenKind::Directive_Endif;
            else if (directive == "else")
                kind = TokenKind::Directive_Else;
            else if (directive == "elif")
                kind = TokenKind::Directive_Elif;
            else if (directive == "undef")
                kind = TokenKind::Directive_Undef;
            else if (directive == "pragma")
                kind = TokenKind::Directive_Pragma;
            else if (directive == "error")
                kind = TokenKind::Directive_Error;
            else if (directive == "warning")
                kind = TokenKind::Directive_Warning;
            else {
                reportError("Unknown preprocessor directive '#" + directive + "'");
                return Token(TokenKind::Invalid);
            }

            Token token(kind);
            token.setText(std::string(source + start, getCurrentPos() - start));
            return token;
        }

        Token Scanner::scanOperatorOrPunctuation() {
            if (isAtEnd()) {
                return Token(TokenKind::Invalid);
            }

            Token result = operatorScanner.scanOperatorOrPunctuation();
            syncScannerPositions();
            return result;
        }

        void Scanner::setPosition(size_t pos, size_t line, size_t column) {
            if (pos > sourceLength) {
                pos = sourceLength;
            }

            identifierScanner.setPosition(pos, line, column);
            operatorScanner.setPosition(pos, line, column);
        }

        bool Scanner::isHexDigit(char c) const {
            return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        }

        bool Scanner::isWhitespace(char c) const {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
        }

        void Scanner::skipWhitespace() {
            while (!isAtEnd() && isWhitespace(getCurrentChar())) {
                updatePosition(getCurrentPos() + 1);
            }
        }

        void Scanner::skipUntilNewline() {
            while (!isAtEnd()) {
                char c = getCurrentChar();
                if (c == '\n') {
                    updatePosition(getCurrentPos() + 1);
                    break;
                }
                updatePosition(getCurrentPos() + 1);
            }
        }

        bool Scanner::lookAhead(const std::string& str) const {
            if (getCurrentPos() + str.length() > sourceLength) {
                return false;
            }

            return std::strncmp(source + getCurrentPos(), str.c_str(), str.length()) == 0;
        }

        std::optional<char> Scanner::peekChar(size_t offset) const {
            size_t pos = getCurrentPos() + offset;
            if (pos >= sourceLength) {
                return std::nullopt;
            }
            return source[pos];
        }

        void Scanner::reportError(const std::string& message) {
            if (diagnostics) {
                std::stringstream ss;
                ss << message << "\n";
                ss << "at position " << getCurrentPos() << " (line " << getCurrentLine() << ", column "
                   << getCurrentColumn() << ")";

                diagnostics->report(
                    DiagnosticLevel::Error,
                    {filename, static_cast<unsigned>(getCurrentLine()), static_cast<unsigned>(getCurrentColumn())},
                    ss.str());
            }
        }

        void Scanner::reportWarning(const std::string& message) {
            if (diagnostics) {
                std::stringstream ss;
                ss << message << "\n";
                ss << "at position " << getCurrentPos() << " (line " << getCurrentLine() << ", column "
                   << getCurrentColumn() << ")";

                diagnostics->report(
                    DiagnosticLevel::Warning,
                    {filename, static_cast<unsigned>(getCurrentLine()), static_cast<unsigned>(getCurrentColumn())},
                    ss.str());
            }
        }

        void Scanner::skipInvalidToken() {
            size_t skippedCount = 0;
            std::string skippedChars;

            while (!isAtEnd() && skippedCount < 10) {
                char c = getCurrentChar();

                // 如果找到可能的token起始，停止跳过
                if (isIdentifierStart(c) || isDigit(c) || c == '"' || c == '\'' || c == '#' || c == '/' ||
                    isWhitespace(c)) {
                    break;
                }

                skippedChars += c;
                updatePosition(getCurrentPos() + 1);
                skippedCount++;
            }

            if (!skippedChars.empty()) {
                reportWarning("Skipped invalid characters: '" + skippedChars + "'");
            }
        }

        void Scanner::syncScannerPositions() {
            size_t idPos = identifierScanner.getCurrentPos();
            size_t opPos = operatorScanner.getCurrentPos();

            if (idPos != opPos) {
                size_t newPos = std::max(idPos, opPos);
                updatePosition(newPos);
            }
        }

        void Scanner::updatePosition(size_t newPos) {
            if (newPos > sourceLength) {
                newPos = sourceLength;
            }

            size_t oldPos = getCurrentPos();
            if (newPos != oldPos) {
                auto [newLine, newColumn] = calculateLineAndColumn(oldPos, newPos);
                setPosition(newPos, newLine, newColumn);
            }
        }

        std::pair<size_t, size_t> Scanner::calculateLineAndColumn(size_t fromPos, size_t toPos) const {
            size_t line = getCurrentLine();
            size_t column = getCurrentColumn();

            for (size_t i = fromPos; i < toPos && i < sourceLength; ++i) {
                char c = source[i];
                if (c == '\n') {
                    line++;
                    column = 1;
                } else if (c == '\r' && i + 1 < sourceLength && source[i + 1] == '\n') {
                    i++;
                    line++;
                    column = 1;
                } else {
                    column++;
                }
            }

            return {line, column};
        }

    }  // namespace frontend
}  // namespace rp
