#pragma once
#include <memory>
#include <string>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Utils/UnicodeEscape.h"

namespace rp {
    namespace frontend {

        class StringLiteralLexer : public Lexer {
          public:
            explicit StringLiteralLexer(DiagnosticEngine *diagEngine = nullptr) : Lexer(diagEngine) {}

            // 扫描字符串字面量并返回对应的Token
            Token scan() {
                std::string input(source + currentPos, sourceLength - currentPos);
                std::string output;
                std::string error;
                size_t pos = 0;

                // 检查是否是原始字符串字面量
                if (input[0] == 'R') {
                    if (processRawStringLiteral(input, output, error)) {
                        Token token(TokenKind::StringLiteral);
                        token.text = output;
                        currentPos += pos;
                        return token;
                    }
                } else {
                    // 处理普通字符串字面量
                    output = processEscapeSequences(input);
                    if (validateStringLiteral(output, error)) {
                        Token token(TokenKind::StringLiteral);
                        token.text = output;
                        currentPos += pos;
                        return token;
                    }
                }

                // 如果处理失败，返回Invalid token
                Token token(TokenKind::Invalid);
                currentPos += pos;
                return token;
            }

            // 处理字符串字面量的转义序列
            static std::string processEscapeSequences(const std::string &raw);

            // 验证字符串字面量的合法性
            static bool validateStringLiteral(const std::string &str, std::string &error);

            // 处理原始字符串字面量 (R"(...)")
            static bool processRawStringLiteral(const std::string &input, std::string &output, std::string &error);

          private:
            // 处理单个转义序列
            static bool processEscapeSequence(const std::string &input, size_t &pos, std::string &output);

            // 处理Unicode转义序列
            static bool processUnicodeEscape(const std::string &input, size_t &pos, std::string &output);
        };

    }  // namespace frontend
}  // namespace rp
