
#pragma once

#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Token/TokenKind.h"

namespace rp {
    namespace frontend {

        class NormalStringProcessor {
          public:
            // 处理常规字符串字面量
            static Token processNormalStringLiteral(const std::string& source,
                                                    size_t& currentPos,
                                                    const SourceLocation& startLoc);

          private:
            // 验证字符串内容的有效性
            static bool validateStringContent(const std::string& content, std::string& error);
        };

    }  // namespace frontend
}  // namespace rp
