#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        Token Lexer::nextToken() {
            if (!tokenCache.empty()) {
                Token token = tokenCache.front();
                tokenCache.pop_front();
                return token;
            }
            return getNextTokenFromSource();
        }

        Token Lexer::peekToken() { return peekToken(1); }

        Token Lexer::peekToken(size_t n) {
            if (n == 0) {
                return Token();  // 返回一个无效token
            }

            // 确保缓存中有足够的token
            fillTokenCache(n);

            // 如果请求的位置超过了可用的token数量，返回EOF
            if (n > tokenCache.size()) {
                return Token(TokenKind::EndOfFile);
            }

            // 返回第n个token（从1开始计数）
            return tokenCache[n - 1];
        }

        void Lexer::ungetToken(const Token& token) {
            // 将token放回缓存的前面
            tokenCache.push_front(token);

            // 如果缓存超过了最大预读数量，移除最后一个token
            if (tokenCache.size() > MAX_LOOKAHEAD) {
                tokenCache.pop_back();
            }
        }

    }  // namespace frontend
}  // namespace rp
