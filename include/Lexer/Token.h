#include "Lexer/TokenKind.h"
namespace rp {
    class Token {
        tok::TokenKind Kind;
        unsigned short Flags;

      public:
        void startToken() { this->Kind = tok::unknown; }

        enum TokenFlags { LeadingSpace = 0x02 };
        void setFlag(TokenFlags Flag) { Flags |= Flag; }
    };
}  // namespace rp