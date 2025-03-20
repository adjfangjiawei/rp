
#include <cctype>
#include <cstring>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        void Lexer::skipWhitespaceAndComments() {
            while (!isAtEnd()) {
                char c = getCurrentChar();
                char next = peekChar();

                // 处理空白字符
                if (std::isspace(c)) {
                    if (c == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                    currentPos++;
                    continue;
                }

                // 处理注释
                if (c == '/' && next == '/') {
                    skipSingleLineComment();
                    continue;
                }

                if (c == '/' && next == '*') {
                    if (!skipMultiLineComment()) {
                        reportError("Unterminated multi-line comment", currentLine, currentColumn);
                    }
                    continue;
                }

                break;
            }
        }

        void Lexer::skipSingleLineComment() {
            currentPos += 2;  // 跳过 //
            currentColumn += 2;

            while (!isAtEnd()) {
                char c = getCurrentChar();
                if (c == '\n') {
                    currentPos++;
                    currentLine++;
                    currentColumn = 1;
                    break;
                }
                currentPos++;
                currentColumn++;
            }
        }

        bool Lexer::skipMultiLineComment() {
            currentPos += 2;  // 跳过 /*
            currentColumn += 2;
            bool foundEnd = false;
            size_t nestingLevel = 1;  // 支持嵌套注释

            while (!isAtEnd()) {
                char c = getCurrentChar();
                char next = peekChar();

                if (c == '/' && next == '*') {
                    nestingLevel++;
                    currentPos += 2;
                    currentColumn += 2;
                    continue;
                }

                if (c == '*' && next == '/') {
                    nestingLevel--;
                    currentPos += 2;
                    currentColumn += 2;
                    if (nestingLevel == 0) {
                        foundEnd = true;
                        break;
                    }
                    continue;
                }

                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
                currentPos++;
            }

            return foundEnd;
        }

        void Lexer::skipUntilNextToken() {
            size_t skippedCount = 0;
            std::string skippedChars;
            bool inError = false;

            while (!isAtEnd() && skippedCount < 100) {  // 防止无限循环
                char c = getCurrentChar();

                // 如果找到可能的token起始，停止跳过
                if (isValidTokenStart(c)) {
                    break;
                }

                // 记录跳过的字符
                if (!inError) {
                    skippedChars += c;
                    skippedCount++;
                }

                // 更新位置
                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                    inError = false;  // 新行重置错误状态
                } else {
                    currentColumn++;
                }
                currentPos++;

                // 如果跳过了太多字符，标记为错误状态
                if (skippedCount >= 10 && !inError) {
                    reportWarning("Skipped invalid characters: '" + skippedChars + "'", currentLine, currentColumn);
                    inError = true;
                }
            }
        }

        bool Lexer::isValidTokenStart(char c) const {
            return std::isalpha(c) || c == '_' || std::isdigit(c) || c == '"' || c == '\'' || c == '#' ||
                   strchr("+-*/%<>=!&|^~.,:;()[]{}\\", c) != nullptr;
        }

    }  // namespace frontend
}  // namespace rp
