
#pragma once
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace rp::frontend::unicode {

    class UnicodeEncoding {
      public:
        // UTF-8编码/解码
        static std::string encodeUtf8(uint32_t codepoint);
        static bool decodeUtf8(const std::string &utf8, std::vector<uint32_t> &codepoints);

        // UTF-8工具函数
        static uint32_t utf8ToCodePoint(std::string_view sv, size_t &bytesRead);
        static std::string codePointToUtf8(uint32_t codePoint);

        // 多字节字符处理
        static std::tuple<uint32_t, size_t> getMultiByteChar(const std::string &str, size_t start);
        static bool isMultiByteChar(const std::string &str, size_t start);
        static size_t getMultiByteCharLength(unsigned char firstByte);
    };

}  // namespace rp::frontend::unicode
