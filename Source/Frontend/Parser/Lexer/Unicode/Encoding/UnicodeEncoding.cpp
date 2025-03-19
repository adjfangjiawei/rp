
#include "UnicodeEncoding.h"

namespace rp::frontend::unicode {

    // UTF-8编码：将Unicode码点转换为UTF-8字符串
    std::string UnicodeEncoding::encodeUtf8(uint32_t codepoint) {
        std::string result;

        if (codepoint <= 0x7F) {
            // 1字节编码
            result.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7FF) {
            // 2字节编码
            result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint <= 0xFFFF) {
            // 3字节编码
            result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint <= 0x10FFFF) {
            // 4字节编码
            result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }

        return result;
    }

    // UTF-8解码：将UTF-8字符串转换为Unicode码点数组
    bool UnicodeEncoding::decodeUtf8(const std::string &utf8, std::vector<uint32_t> &codepoints) {
        codepoints.clear();
        size_t i = 0;

        while (i < utf8.length()) {
            size_t bytesRead;
            uint32_t codepoint = utf8ToCodePoint(std::string_view(utf8.data() + i, utf8.length() - i), bytesRead);

            if (bytesRead == 0) {
                return false;  // 解码错误
            }

            codepoints.push_back(codepoint);
            i += bytesRead;
        }

        return true;
    }

    // 将UTF-8字符转换为Unicode码点
    uint32_t UnicodeEncoding::utf8ToCodePoint(std::string_view sv, size_t &bytesRead) {
        bytesRead = 0;
        if (sv.empty()) return 0;

        unsigned char firstByte = static_cast<unsigned char>(sv[0]);
        size_t length = getMultiByteCharLength(firstByte);

        if (length == 0 || length > sv.length()) {
            return 0;
        }

        uint32_t codepoint;
        if (length == 1) {
            codepoint = firstByte;
        } else {
            // 检查后续字节是否都是合法的UTF-8后续字节（以10开头）
            for (size_t i = 1; i < length; i++) {
                if ((static_cast<unsigned char>(sv[i]) & 0xC0) != 0x80) {
                    return 0;
                }
            }

            // 根据UTF-8编码规则解码
            codepoint = firstByte & ((1 << (8 - length - 1)) - 1);
            for (size_t i = 1; i < length; i++) {
                codepoint = (codepoint << 6) | (static_cast<unsigned char>(sv[i]) & 0x3F);
            }
        }

        bytesRead = length;
        return codepoint;
    }

    // 将Unicode码点转换为UTF-8字符串
    std::string UnicodeEncoding::codePointToUtf8(uint32_t codePoint) { return encodeUtf8(codePoint); }

    // 获取多字节字符及其长度
    std::tuple<uint32_t, size_t> UnicodeEncoding::getMultiByteChar(const std::string &str, size_t start) {
        if (start >= str.length()) {
            return {0, 0};
        }

        size_t bytesRead;
        uint32_t codepoint = utf8ToCodePoint(std::string_view(str.data() + start, str.length() - start), bytesRead);
        return {codepoint, bytesRead};
    }

    // 判断是否是多字节字符的开始
    bool UnicodeEncoding::isMultiByteChar(const std::string &str, size_t start) {
        if (start >= str.length()) {
            return false;
        }

        unsigned char firstByte = static_cast<unsigned char>(str[start]);
        return (firstByte & 0x80) != 0;
    }

    // 获取多字节字符的长度
    size_t UnicodeEncoding::getMultiByteCharLength(unsigned char firstByte) {
        if ((firstByte & 0x80) == 0) {
            return 1;  // ASCII字符
        } else if ((firstByte & 0xE0) == 0xC0) {
            return 2;  // 2字节UTF-8
        } else if ((firstByte & 0xF0) == 0xE0) {
            return 3;  // 3字节UTF-8
        } else if ((firstByte & 0xF8) == 0xF0) {
            return 4;  // 4字节UTF-8
        }
        return 0;  // 无效的UTF-8首字节
    }

}  // namespace rp::frontend::unicode
