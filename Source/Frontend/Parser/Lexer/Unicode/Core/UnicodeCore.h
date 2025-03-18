#pragma once
#include <cstdint>
#include <string>
#include <string_view>

namespace rp::frontend::unicode {

    class UnicodeCore {
      public:
        // 码点验证
        static bool isValidCodepoint(uint32_t codepoint);

        // UTF-8序列验证
        static bool isValidUtf8FirstByte(unsigned char byte);
        static bool isUtf8ContinuationByte(unsigned char byte);
        static bool isOverlongEncoding(uint32_t codepoint, size_t length);

        // UTF-8序列信息
        struct Utf8SequenceInfo {
            size_t length;       // UTF-8序列的字节长度
            uint32_t codepoint;  // 对应的Unicode码点
            bool valid;          // 序列是否有效
            std::string error;   // 错误信息（如果无效）
        };

        static Utf8SequenceInfo getUtf8SequenceInfo(const std::string &str, size_t start);
        static size_t getUtf8ByteCount(uint32_t codepoint);
        static size_t getUtf8SequenceLength(unsigned char firstByte);

      private:
        // UTF-8编码常量
        static constexpr uint8_t UTF8_1BYTE_MASK = 0x80;  // 10000000
        static constexpr uint8_t UTF8_2BYTE_MASK = 0xE0;  // 11100000
        static constexpr uint8_t UTF8_3BYTE_MASK = 0xF0;  // 11110000
        static constexpr uint8_t UTF8_4BYTE_MASK = 0xF8;  // 11111000

        // UTF-8最小值限制
        static constexpr uint32_t UTF8_2BYTE_MIN = 0x80;     // 需要2字节编码的最小值
        static constexpr uint32_t UTF8_3BYTE_MIN = 0x800;    // 需要3字节编码的最小值
        static constexpr uint32_t UTF8_4BYTE_MIN = 0x10000;  // 需要4字节编码的最小值

        // Unicode范围常量
        static constexpr uint32_t UNICODE_MAX = 0x10FFFF;  // Unicode最大码点
        static constexpr uint32_t SURROGATE_MIN = 0xD800;  // 代理对起始
        static constexpr uint32_t SURROGATE_MAX = 0xDFFF;  // 代理对结束
    };

}  // namespace rp::frontend::unicode
