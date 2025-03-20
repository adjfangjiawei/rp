#include "UnicodeCore.h"

namespace rp::frontend::unicode {

    bool UnicodeCore::isValidCodepoint(uint32_t codepoint) {
        // Unicode标准规定的有效码点范围：
        // 1. 码点必须不超过最大值 0x10FFFF
        // 2. 码点不能在代理对范围内 (0xD800-0xDFFF)
        // 3. 码点不能为0xFFFE或0xFFFF（非字符）
        if (codepoint > UNICODE_MAX) {
            return false;
        }
        if (codepoint >= SURROGATE_MIN && codepoint <= SURROGATE_MAX) {
            return false;
        }
        // 检查特殊的非字符码点
        if (codepoint == 0xFFFE || codepoint == 0xFFFF) {
            return false;
        }
        return true;
    }

    bool UnicodeCore::isValidUtf8FirstByte(unsigned char byte) {
        // UTF-8第一个字节的有效模式：
        // 0xxxxxxx (ASCII, 0x00-0x7F)
        // 110xxxxx (2字节序列, 0xC0-0xDF)
        // 1110xxxx (3字节序列, 0xE0-0xEF)
        // 11110xxx (4字节序列, 0xF0-0xF7)

        // ASCII字符
        if (byte < UTF8_1BYTE_MASK) {
            return true;
        }

        // 2字节序列：必须是110xxxxx模式，且值在合理范围内
        if ((byte & UTF8_2BYTE_MASK) == 0xC0) {
            return byte <= 0xDF;
        }

        // 3字节序列：必须是1110xxxx模式
        if ((byte & UTF8_3BYTE_MASK) == 0xE0) {
            return byte <= 0xEF;
        }

        // 4字节序列：必须是11110xxx模式，且值不超过F7
        // （因为Unicode最大码点是0x10FFFF，所以首字节不能超过F4）
        if ((byte & UTF8_4BYTE_MASK) == 0xF0) {
            return byte <= 0xF4;
        }

        return false;
    }

    bool UnicodeCore::isUtf8ContinuationByte(unsigned char byte) {
        // UTF-8后续字节的格式：10xxxxxx (0x80-0xBF)
        // 不仅检查前缀是否为10，还要确保后6位在合法范围内
        const uint8_t CONTINUATION_PREFIX = 0x80;
        const uint8_t CONTINUATION_MASK = 0xC0;
        const uint8_t CONTINUATION_MAX = 0xBF;

        return (byte & CONTINUATION_MASK) == CONTINUATION_PREFIX && byte <= CONTINUATION_MAX;
    }

    bool UnicodeCore::isOverlongEncoding(uint32_t codepoint, size_t length) {
        // 检查是否使用了过长编码
        // UTF-8编码规则要求使用最短可能的字节序列来编码一个码点
        switch (length) {
            case 1:
                // 单字节必须是ASCII（0-0x7F）
                return codepoint >= UTF8_2BYTE_MIN;
            case 2:
                // 2字节序列必须编码 0x80-0x7FF 范围内的码点
                return codepoint < UTF8_2BYTE_MIN || codepoint >= UTF8_3BYTE_MIN;
            case 3:
                // 3字节序列必须编码 0x800-0xFFFF 范围内的码点
                return codepoint < UTF8_3BYTE_MIN || codepoint >= UTF8_4BYTE_MIN;
            case 4:
                // 4字节序列必须编码 0x10000-0x10FFFF 范围内的码点
                return codepoint < UTF8_4BYTE_MIN || codepoint > UNICODE_MAX;
            default:
                // UTF-8不允许超过4字节的序列
                return true;
        }
    }

    UnicodeCore::Utf8SequenceInfo UnicodeCore::getUtf8SequenceInfo(const std::string &str, size_t start) {
        // 边界检查
        if (start >= str.length()) {
            return {0,
                    0,
                    false,
                    "Invalid start position: " + std::to_string(start) +
                        " (string length: " + std::to_string(str.length()) + ")"};
        }

        unsigned char first = static_cast<unsigned char>(str[start]);

        // 获取序列长度并验证首字节
        size_t length = getUtf8SequenceLength(first);
        if (length == 0) {
            return {0, 0, false, "Invalid UTF-8 first byte: 0x" + std::to_string(static_cast<int>(first))};
        }

        // 检查序列完整性
        if (start + length > str.length()) {
            return {0,
                    0,
                    false,
                    "Incomplete UTF-8 sequence at position " + std::to_string(start) + ": expected " +
                        std::to_string(length) + " bytes, but only " + std::to_string(str.length() - start) +
                        " available"};
        }

        uint32_t codepoint = 0;
        std::string error;

        // 根据UTF-8序列长度解码码点
        switch (length) {
            case 1:
                // ASCII字符（0xxxxxxx）
                codepoint = first;
                break;

            case 2:
                {
                    // 2字节序列（110xxxxx 10xxxxxx）
                    unsigned char second = static_cast<unsigned char>(str[start + 1]);
                    if (!isUtf8ContinuationByte(second)) {
                        return {0,
                                0,
                                false,
                                "Invalid continuation byte in 2-byte sequence at position " +
                                    std::to_string(start + 1) + ": 0x" + std::to_string(static_cast<int>(second))};
                    }
                    codepoint = ((first & 0x1F) << 6) | (second & 0x3F);
                    break;
                }

            case 3:
                {
                    // 3字节序列（1110xxxx 10xxxxxx 10xxxxxx）
                    unsigned char second = static_cast<unsigned char>(str[start + 1]);
                    unsigned char third = static_cast<unsigned char>(str[start + 2]);

                    if (!isUtf8ContinuationByte(second)) {
                        return {
                            0,
                            0,
                            false,
                            "Invalid continuation byte in 3-byte sequence at position " + std::to_string(start + 1)};
                    }
                    if (!isUtf8ContinuationByte(third)) {
                        return {
                            0,
                            0,
                            false,
                            "Invalid continuation byte in 3-byte sequence at position " + std::to_string(start + 2)};
                    }

                    codepoint = ((first & 0x0F) << 12) | ((second & 0x3F) << 6) | (third & 0x3F);
                    break;
                }

            case 4:
                {
                    // 4字节序列（11110xxx 10xxxxxx 10xxxxxx 10xxxxxx）
                    unsigned char second = static_cast<unsigned char>(str[start + 1]);
                    unsigned char third = static_cast<unsigned char>(str[start + 2]);
                    unsigned char fourth = static_cast<unsigned char>(str[start + 3]);

                    if (!isUtf8ContinuationByte(second)) {
                        return {
                            0,
                            0,
                            false,
                            "Invalid continuation byte in 4-byte sequence at position " + std::to_string(start + 1)};
                    }
                    if (!isUtf8ContinuationByte(third)) {
                        return {
                            0,
                            0,
                            false,
                            "Invalid continuation byte in 4-byte sequence at position " + std::to_string(start + 2)};
                    }
                    if (!isUtf8ContinuationByte(fourth)) {
                        return {
                            0,
                            0,
                            false,
                            "Invalid continuation byte in 4-byte sequence at position " + std::to_string(start + 3)};
                    }

                    codepoint =
                        ((first & 0x07) << 18) | ((second & 0x3F) << 12) | ((third & 0x3F) << 6) | (fourth & 0x3F);
                    break;
                }
        }

        // 检查过长编码
        if (isOverlongEncoding(codepoint, length)) {
            return {0,
                    0,
                    false,
                    "Overlong UTF-8 encoding detected at position " + std::to_string(start) + ": codepoint " +
                        std::to_string(codepoint) + " encoded using " + std::to_string(length) + " bytes"};
        }

        // 验证解码出的码点是否有效
        if (!isValidCodepoint(codepoint)) {
            return {
                0,
                0,
                false,
                "Invalid Unicode codepoint 0x" + std::to_string(codepoint) + " at position " + std::to_string(start)};
        }

        return {length, codepoint, true, ""};
    }

    size_t UnicodeCore::getUtf8ByteCount(uint32_t codepoint) {
        // 首先验证码点的有效性
        if (!isValidCodepoint(codepoint)) {
            return 0;  // 无效码点返回0
        }

        // 根据Unicode标准确定编码所需的字节数：
        // 0x0000-0x007F: 1字节 (ASCII)
        // 0x0080-0x07FF: 2字节
        // 0x0800-0xFFFF: 3字节
        // 0x10000-0x10FFFF: 4字节
        if (codepoint < UTF8_2BYTE_MIN) {
            return 1;  // ASCII字符
        } else if (codepoint < UTF8_3BYTE_MIN) {
            return 2;  // 基本多语言平面的前半部分
        } else if (codepoint < UTF8_4BYTE_MIN) {
            return 3;  // 基本多语言平面的后半部分
        } else if (codepoint <= UNICODE_MAX) {
            return 4;  // 补充平面
        }

        return 0;  // 超出Unicode范围
    }

    size_t UnicodeCore::getUtf8SequenceLength(unsigned char firstByte) {
        // 根据UTF-8编码规则判断序列长度：
        // 0xxxxxxx: 1字节序列 (ASCII)
        // 110xxxxx: 2字节序列
        // 1110xxxx: 3字节序列
        // 11110xxx: 4字节序列

        // ASCII字符
        if ((firstByte & UTF8_1BYTE_MASK) == 0) {
            return 1;
        }

        // 2字节序列
        if ((firstByte & UTF8_2BYTE_MASK) == 0xC0) {
            // 额外检查确保字节值在合法范围内 (0xC2-0xDF)
            // 0xC0和0xC1是非法的（会导致过长编码）
            return (firstByte >= 0xC2 && firstByte <= 0xDF) ? 2 : 0;
        }

        // 3字节序列
        if ((firstByte & UTF8_3BYTE_MASK) == 0xE0) {
            // 检查是否在合法范围内 (0xE0-0xEF)
            return (firstByte <= 0xEF) ? 3 : 0;
        }

        // 4字节序列
        if ((firstByte & UTF8_4BYTE_MASK) == 0xF0) {
            // 检查是否在合法范围内 (0xF0-0xF4)
            // 0xF5-0xF7会产生超出Unicode范围的码点
            return (firstByte <= 0xF4) ? 4 : 0;
        }

        return 0;  // 无效的UTF-8首字节
    }

}  // namespace rp::frontend::unicode
