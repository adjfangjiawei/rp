
#pragma once

#include "BaseScanner.h"

namespace rp {
    namespace frontend {

        class UTF8Scanner : public BaseScanner {
          public:
            using BaseScanner::BaseScanner;

          protected:
            // UTF-8序列处理
            std::string scanUTF8Sequence();
            bool isValidUTF8Continuation(char c) const { return (static_cast<unsigned char>(c) & 0xC0) == 0x80; }

            // UTF-8错误处理
            void reportInvalidUTF8();
            void skipInvalidUTF8();

            // UTF-8序列解码
            uint32_t decodeUTF8Sequence(char first);
        };

    }  // namespace frontend
}  // namespace rp
