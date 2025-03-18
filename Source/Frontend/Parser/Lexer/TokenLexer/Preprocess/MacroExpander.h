#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class MacroExpander {
          public:
            explicit MacroExpander(DiagnosticEngine* diagEngine);

            // 宏展开
            std::vector<Token> expand(const std::vector<Token>& tokens,
                                      const std::unordered_map<std::string, std::vector<Token>>& paramMap = {}) &&;

            // 字符串化操作
            Token stringize(const std::vector<Token>& tokens) &&;

            // Token连接
            Token concatenate(const Token& left, const Token& right) &&;

            // 处理可变参数
            std::vector<Token> handleVariadicArgs(const std::vector<std::vector<Token>>& args,
                                                  size_t fixedParamCount) &&;

          private:
            DiagnosticEngine* diagnostics;

            // 辅助函数
            bool isStringizingOperator(const Token& token) const;
            bool isConcatenationOperator(const Token& token) const;
            std::string tokenToString(const Token& token) const;
            void reportError(const std::string& message, const Token& token);
        };

    }  // namespace frontend
}  // namespace rp
