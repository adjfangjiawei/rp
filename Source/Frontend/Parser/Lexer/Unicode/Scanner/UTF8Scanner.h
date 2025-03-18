
#pragma once
#include <cstdint>
#include <string>

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {

    class UTF8Scanner {
      public:
        UTF8Scanner(const std::string& input) : input_(input), position_(0) {}

        // 扫描控制
        void reset() { position_ = 0; }
        bool hasMore() const { return position_ < input_.length(); }
        size_t position() const { return position_; }
        void setPosition(size_t pos) { position_ = pos; }

        // UTF-8序列处理
        std::string scanUTF8Sequence();
        uint32_t decodeUTF8Sequence(char first);

        // UTF-8验证
        bool isValidUTF8Continuation(char c) const { return (static_cast<unsigned char>(c) & 0xC0) == 0x80; }

        // 错误处理
        struct ScanError {
            std::string message;
            size_t position;
        };

        ScanError getLastError() const { return lastError_; }

      public:
        // UTF-8错误处理方法
        void skipInvalidUTF8();

        // UTF-8扫描方法
        bool tryPeekCodepoint(uint32_t& codepoint) const;
        size_t lookAhead(size_t n) const;
        std::string peekString(size_t length) const;
        bool skipUntil(uint32_t targetCodepoint);
        std::string collectUntil(uint32_t targetCodepoint);

      protected:
        // 内部扫描方法
        void reportInvalidUTF8(const std::string& message);
        char peek() const { return hasMore() ? input_[position_] : '\0'; }
        char advance() { return hasMore() ? input_[position_++] : '\0'; }

      private:
        const std::string& input_;
        size_t position_;
        ScanError lastError_;
    };

}  // namespace rp::frontend::unicode
