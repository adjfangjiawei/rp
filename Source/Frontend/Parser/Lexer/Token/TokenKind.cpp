#include "TokenKind.h"

namespace rp {
    namespace frontend {

        bool isKeyword(TokenKind kind) {
            int value = static_cast<int>(kind);
            return value >= 100 && value < 200;
        }

        bool isOperator(TokenKind kind) {
            int value = static_cast<int>(kind);
            return value >= 200 && value < 300;
        }

        bool isDelimiter(TokenKind kind) {
            int value = static_cast<int>(kind);
            return value >= 300 && value < 400;
        }

        bool isDirective(TokenKind kind) {
            int value = static_cast<int>(kind);
            return value >= 400 && value < 500;
        }

        bool isTypeKeyword(TokenKind kind) {
            int value = static_cast<int>(kind);
            return value >= 500 && value < 600;
        }

    }  // namespace frontend
}  // namespace rp
