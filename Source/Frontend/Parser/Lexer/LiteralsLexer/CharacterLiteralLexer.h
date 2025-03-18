#pragma once
#include <memory>
#include <string>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Utils/UnicodeEscape.h"

namespace rp {
    namespace frontend {

        class CharacterLiteralLexer : public Lexer {
          public:
            explicit CharacterLiteralLexer(DiagnosticEngine *diagEngine = nullptr) : Lexer(diagEngine) {}

            // 扫描字符字面量并返回对应的Token
            Token scan() {
                std::string input(source + currentPos, sourceLength - currentPos);
                size_t pos = 0;
                long long value = 0;
                std::string error;

                if (processCharacterLiteral(input, pos, value, error)) {
                    Token token(TokenKind::CharLiteral);
                    token.intValue = value;
                    currentPos += pos;
                    return token;
                }

                // 如果处理失败，返回Invalid token
                Token token(TokenKind::Invalid);
                currentPos += pos;
                return token;
            }

            // 处理字符字面量
            static bool processCharacterLiteral(const std::string &input,
                                                size_t &pos,
                                                long long &value,
                                                std::string &error);

            // 验证字符字面量的合法性
            static bool validateCharacterLiteral(const std::string &str, std::string &error);

          private:
            // 处理转义序列
            static bool processEscapeSequence(const std::string &input, size_t &pos, long long &value);

            // 处理Unicode字符
            static bool processUnicodeChar(const std::string &input, size_t &pos, long long &value);

            // 处理UTF-8编码
            static bool processUtf8Encoding(unsigned int codepoint, long long &value);
        };

    }  // namespace frontend
}  // namespace rp
