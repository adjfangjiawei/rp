
#pragma once

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
            bool isIdentifierPart(char c) const;

          private:
            // UTF-8标识符处理
            std::string scanUTF8Identifier();
        };

    }  // namespace frontend
}  // namespace rp
