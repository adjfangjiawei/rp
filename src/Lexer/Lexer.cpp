#include <Lexer/Lexer.h>
using namespace rp;
bool Lexer::Lex(Token &Result) {
    if (HasLeadingSpace) {
        Result.setFlag(Token::LeadingSpace);
    }

    bool returnedToken = this->LexTokenInternal(Result);
    return returnedToken;
}

bool Lexer::LexTokenInternal(Token &Result) { return false; }