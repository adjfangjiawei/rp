#include "MacroExpander.h"

#include <sstream>

namespace rp {
    namespace frontend {

        MacroExpander::MacroExpander(DiagnosticEngine* diagEngine) : diagnostics(diagEngine) {}

        std::vector<Token> MacroExpander::expand(
            const std::vector<Token>& tokens, const std::unordered_map<std::string, std::vector<Token>>& paramMap) && {
            std::vector<Token> result;
            for (size_t i = 0; i < tokens.size(); ++i) {
                const Token& token = tokens[i];

                // 处理字符串化操作符 (#)
                if (isStringizingOperator(token) && i + 1 < tokens.size()) {
                    const Token& nextToken = tokens[i + 1];
                    std::string key(nextToken.text);
                    if (paramMap.find(key) != paramMap.end()) {
                        result.push_back(std::move(*this).stringize(paramMap.at(key)));
                        ++i;  // 跳过下一个token
                        continue;
                    }
                }

                // 处理连接操作符 (##)
                if (isConcatenationOperator(token) && i > 0 && i + 1 < tokens.size()) {
                    const Token& left = result.back();
                    result.pop_back();
                    Token right = Token(tokens[i + 1].kind);
                    right.text = tokens[i + 1].text;
                    right.line = tokens[i + 1].line;
                    right.column = tokens[i + 1].column;
                    right.filename = tokens[i + 1].filename;

                    // 检查是否需要参数替换
                    std::string rightKey(right.text);
                    if (paramMap.find(rightKey) != paramMap.end()) {
                        const auto& paramTokens = paramMap.at(rightKey);
                        if (!paramTokens.empty()) {
                            Token temp = Token(paramTokens.front().kind);
                            temp.text = paramTokens.front().text;
                            temp.line = paramTokens.front().line;
                            temp.column = paramTokens.front().column;
                            temp.filename = paramTokens.front().filename;
                            right = std::move(temp);
                        }
                    }

                    result.push_back(std::move(*this).concatenate(left, std::move(right)));
                    ++i;  // 跳过下一个token
                    continue;
                }

                // 参数替换
                std::string tokenKey(token.text);
                if (paramMap.find(tokenKey) != paramMap.end()) {
                    const auto& replacement = paramMap.at(tokenKey);
                    for (auto&& token : replacement) {
                        result.push_back(std::move(token));
                    }
                    continue;
                }

                // 普通token
                result.push_back(std::move(token));
            }

            return result;
        }

        Token MacroExpander::stringize(const std::vector<Token>& tokens) && {
            std::stringstream ss;
            ss << '"';

            for (const auto& token : tokens) {
                std::string str = tokenToString(token);

                // 转义字符串中的特殊字符
                for (char c : str) {
                    if (c == '"' || c == '\\') {
                        ss << '\\';
                    }
                    ss << c;
                }
            }

            ss << '"';

            Token result;
            result.kind = TokenKind::StringLiteral;
            result.text = ss.str();
            result.line = tokens.empty() ? 0 : tokens.front().line;
            result.column = tokens.empty() ? 0 : tokens.front().column;
            result.filename = tokens.empty() ? "" : std::string(tokens.front().filename);
            return result;
        }

        Token MacroExpander::concatenate(const Token& left, const Token& right) && {
            std::string concatenated = std::string(left.text) + std::string(right.text);

            Token result;
            result.kind = left.kind;  // 临时使用左操作数的类型
            result.text = concatenated;
            result.line = left.line;
            result.column = left.column;
            result.filename = std::string(left.filename);
            return result;
        }

        std::vector<Token> MacroExpander::handleVariadicArgs(const std::vector<std::vector<Token>>& args,
                                                             size_t fixedParamCount) && {
            std::vector<Token> result;

            // 收集所有可变参数
            for (size_t i = fixedParamCount; i < args.size(); ++i) {
                if (i > fixedParamCount) {
                    // 添加逗号分隔符
                    Token comma;
                    comma.kind = TokenKind::Comma;
                    comma.text = ",";
                    result.push_back(comma);
                }

                // 添加参数tokens
                for (auto&& token : args[i]) {
                    result.push_back(std::move(token));
                }
            }

            return result;
        }

        bool MacroExpander::isStringizingOperator(const Token& token) const { return token.kind == TokenKind::Hash; }

        bool MacroExpander::isConcatenationOperator(const Token& token) const {
            return token.kind == TokenKind::HashHash;
        }

        std::string MacroExpander::tokenToString(const Token& token) const {
            // 特殊处理某些token类型
            switch (token.kind) {
                case TokenKind::StringLiteral:
                    // 已经是字符串字面量，需要去掉首尾的引号
                    return std::string(token.text).substr(1, token.text.length() - 2);
                case TokenKind::CharLiteral:
                    // 字符字面量，需要去掉单引号
                    return std::string(token.text).substr(1, token.text.length() - 2);
                default:
                    return std::string(token.text);
            }
        }

        void MacroExpander::reportError(const std::string& message, const Token& token) {
            SourceLocation loc;
            loc.filename = token.filename;
            loc.line = token.line;
            loc.column = token.column;
            diagnostics->report(DiagnosticLevel::Error, loc, message);
        }

    }  // namespace frontend
}  // namespace rp
