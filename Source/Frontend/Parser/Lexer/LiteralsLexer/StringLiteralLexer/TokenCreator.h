
#ifndef RP_TOKEN_CREATOR_H
#define RP_TOKEN_CREATOR_H

#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/StringLiteralUtils.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class TokenCreator {
          public:
            // 创建字符串字面量的Token
            static Token createStringToken(const std::string& content,
                                           StringPrefix prefix,
                                           size_t line,
                                           size_t column,
                                           const std::string& filename);

          private:
            // 设置Token的字符串编码信息
            static void setStringEncoding(Token& token, StringPrefix prefix);

            // 获取对应前缀的Token类型
            static TokenKind getPrefixTokenKind(StringPrefix prefix);

            // 检查数值范围是否有效
            static bool isValidPosition(size_t line, size_t column);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RP_TOKEN_CREATOR_H
