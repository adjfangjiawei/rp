#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"

namespace rp {
    namespace frontend {

        void Lexer::updatePosition(const char* text, size_t length) {
            for (size_t i = 0; i < length; ++i) {
                if (text[i] == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
            }
        }

    }  // namespace frontend
}  // namespace rp
