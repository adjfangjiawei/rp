
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        void Lexer::skipWhitespaceAndComments() {
            while (currentPos < sourceLength) {
                char c = source[currentPos];

                // 跳过空白字符
                if (isspace(c)) {
                    if (c == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                    currentPos++;
                    continue;
                }

                // 跳过单行注释
                if (c == '/' && currentPos + 1 < sourceLength && source[currentPos + 1] == '/') {
                    currentPos += 2;
                    currentColumn += 2;
                    while (currentPos < sourceLength && source[currentPos] != '\n') {
                        currentPos++;
                        currentColumn++;
                    }
                    continue;
                }

                // 跳过多行注释
                if (c == '/' && currentPos + 1 < sourceLength && source[currentPos + 1] == '*') {
                    currentPos += 2;
                    currentColumn += 2;
                    while (currentPos + 1 < sourceLength) {
                        if (source[currentPos] == '*' && source[currentPos + 1] == '/') {
                            currentPos += 2;
                            currentColumn += 2;
                            break;
                        }
                        if (source[currentPos] == '\n') {
                            currentLine++;
                            currentColumn = 1;
                        } else {
                            currentColumn++;
                        }
                        currentPos++;
                    }
                    continue;
                }

                break;
            }
        }

    }  // namespace frontend
}  // namespace rp
