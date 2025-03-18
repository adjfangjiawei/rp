
#pragma once

#include <string_view>

#include "UTF8Scanner.h"

namespace rp {
    namespace frontend {

        class IdentifierScanner : public UTF8Scanner {
          public:
            using UTF8Scanner::UTF8Scanner;

            // 标识符扫描
            Token scanIdentifier();

            // 字符判断函数
            bool isIdentifierStart(char c) const;
            bool isIdentifierContinue(char c) const;  // 重命名为更标准的名称
            bool isUnicodeIdentifierStart(uint32_t codepoint) const;
            bool isUnicodeIdentifierContinue(uint32_t codepoint) const;

          private:
            // UTF-8标识符处理
            std::string scanUTF8Identifier();
            bool validateIdentifier(const std::string& identifier) const;

            // 标识符长度限制
            static constexpr size_t MAX_IDENTIFIER_LENGTH = 1024;

            // 错误处理
            void reportInvalidIdentifier(const std::string& reason);
            void reportIdentifierTooLong();

            // 辅助函数
            bool isValidIdentifierChar(char c) const;
            bool checkIdentifierLength(const std::string& current) const;
        };

    }  // namespace frontend
}  // namespace rp
