
#include "BaseScanner.h"

#include <algorithm>
#include <cstring>

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"
namespace rp {
    namespace frontend {

        BaseScanner::BaseScanner(std::shared_ptr<DiagnosticEngine> diagEngine)
            : source(nullptr),
              sourceLength(0),
              currentPos(0),
              currentLine(1),
              currentColumn(1),
              diagnostics(std::move(diagEngine)) {
            if (!diagnostics) {
                throw std::runtime_error("DiagnosticEngine cannot be null");
            }
        }

        void BaseScanner::setSource(const char* src, size_t length, const std::string& filename) {
            // 增强的输入验证
            if (src == nullptr) {
                if (length > 0) {
                    throw std::invalid_argument("Source pointer cannot be null when length > 0");
                }
                // 确保空输入时有一个有效的空字符串
                source = "";
                sourceLength = 0;
            } else {
                // 验证输入的有效性
                if (length == 0) {
                    source = "";
                    sourceLength = 0;
                } else {
                    // 检查src是否指向有效的字符串
                    try {
                        // 尝试读取第一个字符以验证指针的有效性
                        volatile char testChar = src[0];
                        (void)testChar;  // 防止编译器优化

                        source = src;
                        sourceLength = length;

                        // UTF-8验证
                        std::string error;
                        if (!unicode::UnicodeProcessing::validateUtf8String(std::string(src, length), error)) {
                            // 记录错误但继续处理
                            reportError("Invalid UTF-8 encoding: " + error);
                            // 设置一个标志表示输入可能包含无效的UTF-8序列
                            reportWarning(
                                "Source code contains invalid UTF-8 sequences. Some characters may not be processed "
                                "correctly.");
                        }
                    } catch (const std::exception& e) {
                        throw std::invalid_argument(std::string("Invalid source pointer: ") + e.what());
                    }
                }
            }

            this->filename = filename;
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
        }

        void BaseScanner::setPosition(size_t pos, size_t line, size_t column) {
            if (pos > sourceLength) {
                pos = sourceLength;
            }

            if (pos != currentPos) {
                size_t newLine, newColumn;
                calculateLineAndColumn(pos, newLine, newColumn);
                currentLine = newLine;
                currentColumn = newColumn;
            } else {
                currentLine = line;
                currentColumn = column;
            }

            currentPos = pos;
        }

        void BaseScanner::calculateLineAndColumn(size_t pos, size_t& line, size_t& column) const {
            line = 1;
            column = 1;
            size_t i = 0;

            while (i < pos && i < sourceLength) {
                if (source[i] == '\n') {
                    line++;
                    column = 1;
                    i++;
                } else if (source[i] == '\r' && (i + 1 < sourceLength) && source[i + 1] == '\n') {
                    line++;
                    column = 1;
                    i += 2;
                } else {
                    size_t bytesConsumed;
                    if (unicode::UnicodeProcessing::validateUtf8Sequence(
                            std::string(source + i, sourceLength - i), 0, bytesConsumed)) {
                        uint32_t codepoint;
                        std::string_view sv(source + i, bytesConsumed);
                        codepoint = unicode::UnicodeEncoding::utf8ToCodePoint(sv, bytesConsumed);
                        column += unicode::UnicodeProcessing::getCharWidth(codepoint);
                        i += bytesConsumed;
                    } else {
                        column++;
                        i++;
                    }
                }
            }
        }

        std::pair<size_t, size_t> BaseScanner::getLineAndColumn(size_t pos) const {
            size_t line, column;
            calculateLineAndColumn(pos, line, column);
            return {line, column};
        }

        Token BaseScanner::createToken(TokenKind kind, const std::string& text, size_t startPos) {
            Token token;
            token.kind = kind;
            token.filename = filename;

            auto [tokenLine, tokenColumn] = getLineAndColumn(startPos);
            token.line = tokenLine;
            token.column = tokenColumn;

            if (!text.empty()) {
                token.setText(text);
            } else if (startPos < sourceLength) {
                size_t length = currentPos - startPos;
                if (length == 0) {
                    length = 1;
                }
                if (startPos + length > sourceLength) {
                    length = sourceLength - startPos;
                }
                token.setText(std::string_view(source + startPos, length));
            }

            return token;
        }

        Token BaseScanner::createToken(TokenKind kind, size_t startPos, size_t length) {
            if (startPos >= sourceLength) {
                return createToken(TokenKind::EndOfFile);
            }

            length = std::min(length, sourceLength - startPos);
            return createToken(kind, std::string(source + startPos, length), startPos);
        }

        bool BaseScanner::isAtLineEnd() const {
            if (currentPos >= sourceLength) {
                return true;
            }

            char c = source[currentPos];
            if (c == '\n') {
                return true;
            }

            if (c == '\r' && currentPos + 1 < sourceLength && source[currentPos + 1] == '\n') {
                return true;
            }

            return false;
        }

        void BaseScanner::skipLineEnd() {
            if (currentPos >= sourceLength) {
                return;
            }

            if (source[currentPos] == '\n') {
                currentPos++;
                currentLine++;
                currentColumn = 1;
            } else if (source[currentPos] == '\r' && currentPos + 1 < sourceLength && source[currentPos + 1] == '\n') {
                currentPos += 2;
                currentLine++;
                currentColumn = 1;
            }
        }

        void BaseScanner::skipWhitespace() {
            while (currentPos < sourceLength) {
                size_t bytesConsumed;
                if (unicode::UnicodeProcessing::validateUtf8Sequence(
                        std::string(source + currentPos, sourceLength - currentPos), 0, bytesConsumed)) {
                    uint32_t codepoint;
                    std::string_view sv(source + currentPos, bytesConsumed);
                    codepoint = unicode::UnicodeEncoding::utf8ToCodePoint(sv, bytesConsumed);

                    if (unicode::UnicodeCategories::isWhitespace(codepoint)) {
                        if (codepoint == '\n') {
                            currentPos++;
                            currentLine++;
                            currentColumn = 1;
                        } else if (codepoint == '\r' && currentPos + 1 < sourceLength &&
                                   source[currentPos + 1] == '\n') {
                            currentPos += 2;
                            currentLine++;
                            currentColumn = 1;
                        } else {
                            currentPos += bytesConsumed;
                            currentColumn += unicode::UnicodeProcessing::getCharWidth(codepoint);
                        }
                    } else {
                        break;
                    }
                } else {
                    char c = source[currentPos];
                    if (c == ' ' || c == '\t') {
                        currentPos++;
                        currentColumn++;
                    } else if (c == '\n') {
                        currentPos++;
                        currentLine++;
                        currentColumn = 1;
                    } else if (c == '\r' && currentPos + 1 < sourceLength && source[currentPos + 1] == '\n') {
                        currentPos += 2;
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        break;
                    }
                }
            }
        }

        bool BaseScanner::matchString(const char* str, size_t length) const {
            if (currentPos + length > sourceLength) {
                return false;
            }

            return std::strncmp(source + currentPos, str, length) == 0;
        }

        void BaseScanner::reportError(const std::string& message) {
            SourceLocation loc{filename, static_cast<unsigned>(currentLine), static_cast<unsigned>(currentColumn)};
            diagnostics->report(DiagnosticLevel::Error, loc, message);
        }

        void BaseScanner::reportWarning(const std::string& message) {
            SourceLocation loc{filename, static_cast<unsigned>(currentLine), static_cast<unsigned>(currentColumn)};
            diagnostics->report(DiagnosticLevel::Warning, loc, message);
        }

    }  // namespace frontend
}  // namespace rp
