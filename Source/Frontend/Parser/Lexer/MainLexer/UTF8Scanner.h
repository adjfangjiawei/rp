
#pragma once

#include "BaseScanner.h"
#include "Frontend/Parser/Lexer/Unicode/Scanner/UTF8Scanner.h"

namespace rp {
    namespace frontend {

        class UTF8Scanner : public BaseScanner {
          public:
            using BaseScanner::BaseScanner;

          protected:
            // UTF-8序列处理
            std::string scanUTF8Sequence();
            bool isValidUTF8Continuation(char c) const { return unicodeScanner->isValidUTF8Continuation(c); }

            // UTF-8错误处理
            void reportInvalidUTF8();
            void skipInvalidUTF8();

            // UTF-8序列解码
            uint32_t decodeUTF8Sequence(char first);

          private:
            // 用于复用Unicode模块的功能
            std::unique_ptr<unicode::UTF8Scanner> unicodeScanner;
        };

    }  // namespace frontend
}  // namespace rp
