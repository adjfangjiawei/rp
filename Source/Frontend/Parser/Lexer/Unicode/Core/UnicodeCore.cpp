#include "UnicodeCore.h"

namespace rp::frontend::unicode {

    bool UnicodeCore::isValidCodepoint(uint32_t codepoint) {
        // Unicode标准规定的有效码点范围
        return (codepoint <= UNICODE_MAX) &&                                 // 不超过最大Unicode码点
               !(codepoint >= SURROGATE_MIN && codepoint <= SURROGATE_MAX);  // 不在代理对范围内
    }

    bool UnicodeCore::isValidUtf8FirstByte(unsigned char byte) {
        // UTF-8第一个字节的有效模式：
        // 0xxxxxxx (ASCII)
        // 110xxxxx (2字节序列)
        // 1110xxxx (3字节序列)
        // 11110xxx (4字节序列)
        return (byte < UTF8_1BYTE_MASK) ||            // ASCII
               ((byte & UTF8_2BYTE_MASK) == 0xC0) ||  // 2字节序列
               ((byte & UTF8_3BYTE_MASK) == 0xE0) ||  // 3字节序列
               ((byte & UTF8_4BYTE_MASK) == 0xF0);    // 4字节序列
    }

    bool UnicodeCore::isUtf8ContinuationByte(unsigned char byte) {
        // UTF-8后续字节的格式：10xxxxxx
        return (byte & 0xC0) == 0x80;
    }

    bool UnicodeCore::isOverlongEncoding(uint32_t codepoint, size_t length) {
        // 检查是否使用了过长编码
        switch (length) {
            case 1:
                return codepoint >= UTF8_2BYTE_MIN;
            case 2:
                return codepoint < UTF8_2BYTE_MIN;
            case 3:
                return codepoint < UTF8_3BYTE_MIN;
            case 4:
                return codepoint < UTF8_4BYTE_MIN;
            default:
                return true;
        }
    }

    UnicodeCore::Utf8SequenceInfo UnicodeCore::getUtf8SequenceInfo(const std::string &str, size_t start) {
        if (start >= str.length()) {
            return {0, 0, false, "Invalid start position: beyond string length"};
        }

        unsigned char first = static_cast<unsigned char>(str[start]);
        size_t length = getUtf8SequenceLength(first);

        if (length == 0) {
            return {0, 0, false, "Invalid UTF-8 first byte: " + std::to_string(first)};
        }

        if (start + length > str.length()) {
            return {0, 0, false, "Incomplete UTF-8 sequence: expected " + std::to_string(length) + " bytes"};
        }

        uint32_t codepoint = 0;

        // 根据UTF-8序列长度解码码点
        switch (length) {
            case 1:
                codepoint = first;
                break;

            case 2:
                if (!isUtf8ContinuationByte(str[start + 1])) {
                    return {0, 0, false, "Invalid continuation byte in 2-byte sequence"};
                }
                codepoint = ((first & 0x1F) << 6) | (static_cast<unsigned char>(str[start + 1]) & 0x3F);
                break;

            case 3:
                if (!isUtf8ContinuationByte(str[start + 1]) || !isUtf8ContinuationByte(str[start + 2])) {
                    return {0, 0, false, "Invalid continuation byte in 3-byte sequence"};
                }
                codepoint = ((first & 0x0F) << 12) | ((static_cast<unsigned char>(str[start + 1]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(str[start + 2]) & 0x3F);
                break;

            case 4:
                if (!isUtf8ContinuationByte(str[start + 1]) || !isUtf8ContinuationByte(str[start + 2]) ||
                    !isUtf8ContinuationByte(str[start + 3])) {
                    return {0, 0, false, "Invalid continuation byte in 4-byte sequence"};
                }
                codepoint = ((first & 0x07) << 18) | ((static_cast<unsigned char>(str[start + 1]) & 0x3F) << 12) |
                            ((static_cast<unsigned char>(str[start + 2]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(str[start + 3]) & 0x3F);
                break;
        }

        // 检查过长编码
        if (isOverlongEncoding(codepoint, length)) {
            return {0, 0, false, "Overlong UTF-8 encoding detected"};
        }

        // 验证解码出的码点是否有效
        if (!isValidCodepoint(codepoint)) {
            return {0, 0, false, "Invalid Unicode codepoint: " + std::to_string(codepoint)};
        }

        return {length, codepoint, true, ""};
    }

    size_t UnicodeCore::getUtf8ByteCount(uint32_t codepoint) {
        if (!isValidCodepoint(codepoint)) return 0;  // 无效码点
        if (codepoint < UTF8_2BYTE_MIN) return 1;
        if (codepoint < UTF8_3BYTE_MIN) return 2;
        if (codepoint < UTF8_4BYTE_MIN) return 3;
        return 4;
    }

    size_t UnicodeCore::getUtf8SequenceLength(unsigned char firstByte) {
        if ((firstByte & UTF8_1BYTE_MASK) == 0) return 1;
        if ((firstByte & UTF8_2BYTE_MASK) == 0xC0) return 2;
        if ((firstByte & UTF8_3BYTE_MASK) == 0xE0) return 3;
        if ((firstByte & UTF8_4BYTE_MASK) == 0xF0) return 4;
        return 0;  // 无效的UTF-8首字节
    }

}  // namespace rp::frontend::unicode
