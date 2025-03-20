#include "UnicodeEncoding.h"

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {

    std::string UnicodeEncoding::encodeUtf8(uint32_t codepoint) {
        // 首先验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return "";  // 返回空字符串表示无效码点
        }

        std::string result;
        result.reserve(4);  // 预分配最大可能需要的空间

        if (codepoint <= 0x7F) {
            // 1字节编码 (0xxxxxxx)
            result.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7FF) {
            // 2字节编码 (110xxxxx 10xxxxxx)
            result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint <= 0xFFFF) {
            // 3字节编码 (1110xxxx 10xxxxxx 10xxxxxx)
            result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else {
            // 4字节编码 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
            result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }

        return result;
    }

    bool UnicodeEncoding::decodeUtf8(const std::string &utf8, std::vector<uint32_t> &codepoints) {
        codepoints.clear();
        codepoints.reserve(utf8.length());  // 预分配空间（最大可能需要的大小）

        size_t i = 0;
        while (i < utf8.length()) {
            size_t bytesRead;
            uint32_t codepoint = utf8ToCodePoint(std::string_view(utf8.data() + i, utf8.length() - i), bytesRead);

            if (bytesRead == 0 || !UnicodeCore::isValidCodepoint(codepoint)) {
                codepoints.clear();
                return false;  // 解码错误或无效码点
            }

            codepoints.push_back(codepoint);
            i += bytesRead;
        }

        return true;
    }

    uint32_t UnicodeEncoding::utf8ToCodePoint(std::string_view sv, size_t &bytesRead) {
        bytesRead = 0;
        if (sv.empty()) return 0;

        unsigned char firstByte = static_cast<unsigned char>(sv[0]);
        size_t length = getMultiByteCharLength(firstByte);

        if (length == 0 || length > sv.length()) {
            return 0;  // 无效的UTF-8首字节或不完整的序列
        }

        uint32_t codepoint;
        if (length == 1) {
            codepoint = firstByte;
        } else {
            // 检查后续字节
            for (size_t i = 1; i < length; i++) {
                if (!UnicodeCore::isUtf8ContinuationByte(static_cast<unsigned char>(sv[i]))) {
                    return 0;  // 无效的后续字节
                }
            }

            // 解码多字节序列
            switch (length) {
                case 2:
                    codepoint = ((firstByte & 0x1F) << 6) | (static_cast<unsigned char>(sv[1]) & 0x3F);
                    if (codepoint < 0x80) return 0;  // 过长编码
                    break;
                case 3:
                    codepoint = ((firstByte & 0x0F) << 12) | ((static_cast<unsigned char>(sv[1]) & 0x3F) << 6) |
                                (static_cast<unsigned char>(sv[2]) & 0x3F);
                    if (codepoint < 0x800) return 0;  // 过长编码
                    break;
                case 4:
                    codepoint = ((firstByte & 0x07) << 18) | ((static_cast<unsigned char>(sv[1]) & 0x3F) << 12) |
                                ((static_cast<unsigned char>(sv[2]) & 0x3F) << 6) |
                                (static_cast<unsigned char>(sv[3]) & 0x3F);
                    if (codepoint < 0x10000) return 0;  // 过长编码
                    break;
                default:
                    return 0;  // 不应该发生
            }
        }

        // 验证解码出的码点
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return 0;
        }

        bytesRead = length;
        return codepoint;
    }

    std::string UnicodeEncoding::codePointToUtf8(uint32_t codePoint) { return encodeUtf8(codePoint); }

    std::tuple<uint32_t, size_t> UnicodeEncoding::getMultiByteChar(const std::string &str, size_t start) {
        if (start >= str.length()) {
            return {0, 0};
        }

        size_t bytesRead;
        uint32_t codepoint = utf8ToCodePoint(std::string_view(str.data() + start, str.length() - start), bytesRead);
        return {codepoint, bytesRead};
    }

    bool UnicodeEncoding::isMultiByteChar(const std::string &str, size_t start) {
        if (start >= str.length()) {
            return false;
        }

        unsigned char firstByte = static_cast<unsigned char>(str[start]);
        return UnicodeCore::isValidUtf8FirstByte(firstByte) && (firstByte & 0x80) != 0;
    }

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
