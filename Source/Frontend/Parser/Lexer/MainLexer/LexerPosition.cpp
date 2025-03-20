
#include <cstring>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        char Lexer::peekChar(size_t offset) const {
            size_t pos = currentPos + offset;
            return (pos < sourceLength) ? source[pos] : '\0';
        }

        bool Lexer::matchString(const std::string& str) const {
            if (currentPos + str.length() > sourceLength) {
                return false;
            }
            return std::strncmp(source + currentPos, str.c_str(), str.length()) == 0;
        }

        void Lexer::recoverFromError() {
            // 尝试同步到下一个有效的token开始位置
            skipUntilNextToken();

            // 确保我们至少前进了一个字符
            if (currentPos == tokenStart) {
                currentPos++;
                if (getCurrentChar() == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
            }
        }

    }  // namespace frontend
}  // namespace rp
