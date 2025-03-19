#pragma once
#include <memory>
#include <string>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Unicode/Core/UnicodeCore.h"
#include "Frontend/Parser/Lexer/Unicode/Escape/UnicodeEscape.h"

namespace rp {
    namespace frontend {

        class CharacterLiteralLexer : public Lexer {
          public:
            explicit CharacterLiteralLexer(std::shared_ptr<DiagnosticEngine> diagEngine = nullptr)
                : Lexer(diagEngine) {}

            // 扫描字符字面量并返回对应的Token
            Token scan() {
                if (currentPos >= sourceLength) {
                    return createToken(TokenKind::Invalid);
                }

                // 保存token的起始位置
                saveTokenStart();

                // 检查是否是字符字面量的开始
                if (source[currentPos] != '\'') {
                    currentPos++;  // 确保至少前进一个位置
                    return createToken(TokenKind::Invalid);
                }

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

                // 如果处理失败，设置诊断信息
                if (auto diag = getDiagnostics()) {
                    SourceLocation loc;
                    loc.line = currentLine;
                    loc.column = currentColumn;
                    loc.filename = filename;
                    diag->report(DiagnosticLevel::Error, loc, "Error in character literal: " + error);
                }

                // 错误恢复：找到下一个单引号或至少前进一个字符
                size_t nextQuote = input.find('\'', 1);
                if (nextQuote != std::string::npos) {
                    currentPos += nextQuote + 1;
                } else {
                    currentPos += 1;
                }

                return createToken(TokenKind::Invalid);
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
        };

    }  // namespace frontend
}  // namespace rp
