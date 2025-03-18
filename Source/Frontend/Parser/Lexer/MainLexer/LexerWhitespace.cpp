
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
            size_t originalPos = currentPos;
            size_t skippedCount = 0;
            std::string skippedChars;
            bool inError = false;
            bool foundValidToken = false;

            while (!isAtEnd() && skippedCount < 100) {  // 防止无限循环
                char c = getCurrentChar();
                char next = peekChar();

                // 特殊处理字符串相关的字符
                if (c == '"' || c == '\'' || (c == 'R' && next == '"') ||
                    ((c == 'L' || c == 'u' || c == 'U') && (next == '"' || next == '\''))) {
                    foundValidToken = true;
                    break;
                }

                // 处理换行符
                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                    currentPos++;
                    skippedCount++;
                    inError = false;  // 新行重置错误状态
                    continue;
                }

                // 如果找到可能的token起始，停止跳过
                if (isValidTokenStart(c)) {
                    foundValidToken = true;
                    break;
                }

                // 记录跳过的字符
                if (!inError && !std::isspace(c)) {
                    skippedChars += c;
                    skippedCount++;
                }

                // 更新位置
                currentColumn++;
                currentPos++;

                // 如果跳过了太多字符，标记为错误状态
                if (skippedCount >= 10 && !inError) {
                    reportWarning("Skipped invalid characters: '" + skippedChars + "'", currentLine, currentColumn);
                    inError = true;
                }
            }

            // 确保至少前进了一个字符
            if (currentPos <= originalPos || !foundValidToken) {
                currentPos = originalPos + 1;
                if (currentPos < sourceLength && source[originalPos] == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
            }
        }

    }  // namespace frontend
}  // namespace rp
