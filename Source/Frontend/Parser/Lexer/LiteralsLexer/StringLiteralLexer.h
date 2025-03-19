#pragma once
#include <memory>
#include <string>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Unicode/Core/UnicodeCore.h"
#include "Frontend/Parser/Lexer/Unicode/Escape/UnicodeEscape.h"

namespace rp {
    namespace frontend {

        class StringLiteralLexer : public Lexer {
          public:
            explicit StringLiteralLexer(std::shared_ptr<DiagnosticEngine> diagEngine = nullptr) : Lexer(diagEngine) {}

            // 扫描字符串字面量并返回对应的Token
            Token scan() {
                if (currentPos >= sourceLength) {
                    return createToken(TokenKind::Invalid);
                }

                // 保存token的起始位置
                saveTokenStart();

                std::string input(source + currentPos, sourceLength - currentPos);
                std::string output;
                std::string error;
                size_t consumed = 0;

                bool success = false;

                // 检查是否是原始字符串字面量
                if (input[0] == 'R' && input.length() > 1 && input[1] == '"') {
                    success = processRawStringLiteral(input, output, consumed, error);
                    if (success) {
                        currentPos += consumed;
                        return createToken(TokenKind::StringLiteral, output);
                    }
                } else if (input[0] == '"') {
                    // 处理普通字符串字面量
                    success = processStringLiteral(input, output, consumed, error);
                    if (success) {
                        currentPos += consumed;
                        return createToken(TokenKind::StringLiteral, output);
                    }
                } else {
                    error = "Expected string literal to start with '\"' or R\"";
                }

                if (!success) {
                    // 设置诊断信息
                    if (auto diag = getDiagnostics()) {
                        SourceLocation loc;
                        loc.line = currentLine;
                        loc.column = currentColumn;
                        loc.filename = filename;
                        diag->report(DiagnosticLevel::Error, loc, "Error in string literal: " + error);
                    }

                    // 改进的错误恢复逻辑
                    size_t errorRecoveryPos = currentPos;
                    bool foundEnd = false;

                    // 从当前位置开始寻找字符串的结束
                    while (errorRecoveryPos < sourceLength) {
                        if (source[errorRecoveryPos] == '"') {
                            // 找到引号，检查前面是否是未转义的反斜杠
                            if (errorRecoveryPos > 0 && source[errorRecoveryPos - 1] == '\\') {
                                size_t backslashCount = 0;
                                size_t temp = errorRecoveryPos - 1;
                                while (temp >= currentPos && source[temp] == '\\') {
                                    backslashCount++;
                                    temp--;
                                }
                                if (backslashCount % 2 == 0) {
                                    // 偶数个反斜杠，这是一个真正的结束引号
                                    foundEnd = true;
                                    errorRecoveryPos++;
                                    break;
                                }
                            } else {
                                // 没有前导反斜杠，这是一个真正的结束引号
                                foundEnd = true;
                                errorRecoveryPos++;
                                break;
                            }
                        } else if (source[errorRecoveryPos] == '\n' || source[errorRecoveryPos] == '\r') {
                            // 遇到换行符，直接跳到下一行开始
                            foundEnd = true;
                            errorRecoveryPos++;
                            break;
                        }
                        errorRecoveryPos++;
                    }

                    // 如果没有找到合适的结束位置，至少前进一个字符
                    if (!foundEnd) {
                        errorRecoveryPos = currentPos + 1;
                    }

                    // 更新当前位置
                    currentPos = errorRecoveryPos;
                    return createToken(TokenKind::Invalid);
                }

                return createToken(TokenKind::Invalid);
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
        };

    }  // namespace frontend
}  // namespace rp
