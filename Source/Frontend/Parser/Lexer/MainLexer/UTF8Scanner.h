
#pragma once

#include <optional>

#include "BaseScanner.h"
#include "Frontend/Parser/Lexer/Unicode/Scanner/UTF8Scanner.h"

namespace rp {
    namespace frontend {

        class UTF8Scanner : public BaseScanner {
          public:
            explicit UTF8Scanner(std::shared_ptr<DiagnosticEngine> diagEngine);
            ~UTF8Scanner() override = default;

            // 重写基类方法
            void setSource(const char* src, size_t length, const std::string& filename) override;

          protected:
            // UTF-8序列处理
            std::string scanUTF8Sequence();
            bool isValidUTF8Continuation(char c) const;
            bool isValidUTF8FirstByte(char c) const;
            size_t getUTF8SequenceLength(char firstByte) const;

            // UTF-8字符检查
            bool isUTF8Char() const;
            std::optional<uint32_t> tryPeekCodepoint() const;
            size_t lookAheadUTF8(size_t n) const;

            // UTF-8错误处理
            void reportInvalidUTF8(const std::string& detail = "");
            void skipInvalidUTF8();

            // UTF-8序列解码
            uint32_t decodeUTF8Sequence(char first);
            std::pair<uint32_t, size_t> getNextCodepoint();

            // UTF-8字符串处理
            std::string collectUTF8Until(uint32_t targetCodepoint);
            bool skipUTF8Until(uint32_t targetCodepoint);

          private:
            // Unicode处理器
            std::unique_ptr<unicode::UTF8Scanner> unicodeScanner;

            // 初始化Unicode处理器
            void initUnicodeScanner();

            // 禁用拷贝和赋值
            UTF8Scanner(const UTF8Scanner&) = delete;
            UTF8Scanner& operator=(const UTF8Scanner&) = delete;
        };

    }  // namespace frontend
}  // namespace rp
