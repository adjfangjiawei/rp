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
                if (currentPos >= sourceLength) {
                    return Token(TokenKind::Invalid);
                }

                std::string input(source + currentPos, sourceLength - currentPos);
                std::string output;
                std::string error;
                size_t consumed = 0;

                Token token(TokenKind::Invalid);
                bool success = false;

                // 检查是否是原始字符串字面量
                if (input[0] == 'R' && input.length() > 1 && input[1] == '"') {
                    success = processRawStringLiteral(input, output, consumed, error);
                    if (success) {
                        token = Token(TokenKind::StringLiteral);
                        token.text = output;
                    }
                } else if (input[0] == '"') {
                    // 处理普通字符串字面量
                    success = processStringLiteral(input, output, consumed, error);
                    if (success) {
                        token = Token(TokenKind::StringLiteral);
                        token.text = output;
                    }
                } else {
                    error = "Expected string literal to start with '\"' or R\"";
                }

                if (!success) {
                    // 设置诊断信息
                    if (auto diag = getDiagnostics()) {
                        SourceLocation loc;
                        loc.line = 1;  // 由于我们只有位置信息，暂时使用默认值
                        loc.column = currentPos + 1;
                        loc.filename = filename;
                        diag->report(DiagnosticLevel::Error, loc, "Error in string literal: " + error);
                    }
                    // 尝试错误恢复：找到下一个引号
                    size_t nextQuote = input.find('"', 1);
                    if (nextQuote != std::string::npos) {
                        consumed = nextQuote + 1;
                    } else {
                        consumed = 1;  // 至少前进一个字符
                    }
                }

                currentPos += consumed;
                return token;
            }

            // 处理普通字符串字面量
            static bool processStringLiteral(const std::string &input,
                                             std::string &output,
                                             size_t &consumed,
                                             std::string &error);

            // 处理原始字符串字面量 (R"delim(...)delim")
            static bool processRawStringLiteral(const std::string &input,
                                                std::string &output,
                                                size_t &consumed,
                                                std::string &error);

            // 验证字符串字面量的合法性
            static bool validateStringLiteral(const std::string &str, std::string &error);

          private:
            // 处理转义序列
            static bool processEscapeSequence(const std::string &input,
                                              size_t &pos,
                                              std::string &output,
                                              std::string &error);

            // 验证UTF-8序列的合法性
            static bool validateUtf8Sequence(const std::string &str, size_t start, size_t &bytesConsumed);
        };

    }  // namespace frontend
}  // namespace rp
